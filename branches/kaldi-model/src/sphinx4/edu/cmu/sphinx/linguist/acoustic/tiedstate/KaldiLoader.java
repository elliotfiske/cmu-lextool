/*
 * Copyright 2013 Carnegie Mellon University.  
 * Portions Copyright 2004 Sun Microsystems, Inc.  
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 */

package edu.cmu.sphinx.linguist.acoustic.tiedstate;

import java.io.*;
import java.net.*;
import java.util.*;

import edu.cmu.sphinx.linguist.acoustic.*;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi.*;

import edu.cmu.sphinx.util.props.*;
import edu.cmu.sphinx.util.ExtendedStreamTokenizer;
import edu.cmu.sphinx.util.LogMath;


final class HmmState {

    private final int id;
    private final int pdfClass;
    private final List<Integer> transitions;

    public HmmState(int id, int pdfClass, Collection<Integer> transitions) {
        this.id = id;
        this.pdfClass = pdfClass;
        this.transitions = new ArrayList<Integer>(transitions);
    }

    public int getId() {
        return id;
    }

    public int getPdfClass() {
        return pdfClass;
    }

    public List<Integer> getTransitions() {
        return transitions;
    }

    public int size() {
        return transitions.size();
    }

    @Override
    public String toString() {
        return String.format("HmmSate {%d, %d, %s}",
                             id, pdfClass, transitions);
    }
}

class Triple {

    private int phone;
    private int hmmState;
    private int pdf;

    public Triple(int phone, int hmmState, int pdf) {
        this.phone = phone;
        this.hmmState = hmmState;
        this.pdf = pdf;
    }

    @Override
    public boolean equals(Object object) {
        if (!(object instanceof Triple))
            return false;

        Triple other = (Triple) object;

        return phone    == other.phone &&
               hmmState == other.hmmState &&
               pdf      == other.pdf;
    }

    @Override
    public int hashCode() {
        return 31 * (31 * phone + hmmState) + pdf;
    }

    @Override
    public String toString() {
        return String.format("Triple {%d, %d, %d}", phone, hmmState, pdf);
    }
}

public class KaldiLoader implements Loader {

    private final class AcousticModelParser {

        private final Scanner scanner;

        // phone -> topology
        private Map<Integer, List<HmmState>> phoneStates;
        private Map<Triple, Integer> transitionStates;
        private List<Float> logProbabilities;
        private List<DiagGmm> mixtures;
        private EventMap eventMap;

        private int contextWidth;
        private int contextPosition;

        public AcousticModelParser(String path)
            throws IOException, MalformedURLException
        {
            // TODO: rewrite with StreamTokenizer, see ExtendedStreamTokenizer.
            File modelFile = new File(path, "final.mdl");
            InputStream modelStream = new URL(modelFile.getPath()).openStream();
            File treeFile = new File(path, "tree");
            InputStream treeStream = new URL(treeFile.getPath()).openStream();

            InputStream s = new SequenceInputStream(modelStream, treeStream);
            scanner = new Scanner(s);

            try {
                parseModel();
                parseTree();
            } catch (InputMismatchException e) {
                // TODO: refactor
                throw e;
            } catch (NoSuchElementException e) {
                throw new InputMismatchException("unexpected end of input");
            }
        }

        private void parseModel() {
            parseTransitionModel();
            expectToken("<DIMENSION>");
            // Skip dimension value.
            scanner.next();
            expectToken("<NUMPDFS>");
            int npdf = scanner.nextInt();

            mixtures = new ArrayList<DiagGmm>(npdf);
            for (int i = 0; i < npdf; ++i) parseGmm(i);
        }

        private void parseTree() {
            expectToken("ContextDependency");
            contextWidth = scanner.nextInt();
            contextPosition = scanner.nextInt();
            expectToken("ToPdf");
            eventMap = parseEventMap();
            expectToken("EndContextDependency");
        }

        private EventMap parseEventMap() {
            String token = scanner.next();

            if ("CE".equals(token))
                return new ConstantEventMap(scanner.nextInt());

            if ("SE".equals(token))
                return parseSplitEventMap();

            if ("TE".equals(token))
                return parseTableEventMap();

            if ("NULL".equals(token))
                return null;

            throw new InputMismatchException(token);
        }

        private EventMap parseSplitEventMap() {
            int key = scanner.nextInt();
            Collection<Integer> answers = new ArrayList<Integer>();

            for (String token : parseTokenList("[", "]"))
                answers.add(Integer.valueOf(token));

            expectToken("{");
            EventMap yesMap = parseEventMap();
            EventMap noMap = parseEventMap();
            EventMap eventMap = new SplitEventMap(key, answers, yesMap, noMap);
            expectToken("}");

            return eventMap;
        }

        private EventMap parseTableEventMap() {
            int key = scanner.nextInt();
            int size = scanner.nextInt();
            List<EventMap> table = new ArrayList<EventMap>(size);

            expectToken("(");

            while (0 < size--)
                table.add(parseEventMap());

            expectToken(")");
            return new TableEventMap(key, table);
        }

        private void parseTransitionModel() {
            expectToken("<TransitionModel>");
            parseTopology();

            expectToken("<Triples>");
            transitionStates = new HashMap<Triple, Integer>();
            int numTriples = scanner.nextInt();
            int transitionId = 1;

            for (int i = 0; i < numTriples; ++i) {
                int phone = scanner.nextInt();
                int hmmState = scanner.nextInt();
                int pdf = scanner.nextInt();
                Triple triple = new Triple(phone, hmmState, pdf);
                transitionStates.put(triple, transitionId);
                transitionId +=
                    phoneStates.get(phone).get(hmmState).getTransitions().size();
            }

            expectToken("</Triples>");
            expectToken("<LogProbs>");
            logProbabilities = parseFloatList();
            expectToken("</LogProbs>");
            expectToken("</TransitionModel>");
        }

        public int getTransitionIndex(Triple triple) {
            return transitionStates.get(triple);
        }

        public float getProbability(int i) {
            return logProbabilities.get(i);
        }

        private void parseTopology() {
            expectToken("<Topology>");

            phoneStates = new HashMap<Integer, List<HmmState>>();
            String token;

            while ("<TopologyEntry>".equals(token = scanner.next())) {
                assertToken("<TopologyEntry>", token);
                expectToken("<ForPhones>");

                List<Integer> phones = new ArrayList<Integer>();
                while (!"</ForPhones>".equals(token = scanner.next()))
                    phones.add(Integer.parseInt(token));

                List<HmmState> states = new ArrayList<HmmState>(3);
                while ("<State>".equals(token = scanner.next())) {
                    // Skip state number.
                    int id = scanner.nextInt();
                    token = scanner.next();

                    if ("<PdfClass>".equals(token)) {
                        int pdfClass = scanner.nextInt();
                        List<Integer> transitions = new ArrayList<Integer>();
                        while ("<Transition>".equals(token = scanner.next())) {
                            transitions.add(scanner.nextInt());
                            // Skip initial probability.
                            scanner.next();
                        }

                        assertToken("</State>", token);
                        states.add(new HmmState(id, pdfClass, transitions));
                    }
                }

                for (Integer id : phones)
                    phoneStates.put(id, states);
            }

            assertToken("</Topology>", token);
        }

        private void parseGmm(int i) {
            expectToken("<DiagGMM>");
            expectToken("<GCONSTS>");
            List<Float> gconsts = parseFloatList();

            expectToken("<WEIGHTS>");
            List<Float> weights = parseFloatList();

            expectToken("<MEANS_INVVARS>");
            List<Float> means = parseFloatList();

            expectToken("<INV_VARS>");
            List<Float> vars = parseFloatList();
            expectToken("</DiagGMM>");

            mixtures.add(new DiagGmm(i, gconsts, means, vars));
        }

        private List<Float> parseFloatList() {
            List<Float> result = new ArrayList<Float>();
            for (String token : parseTokenList("[", "]"))
                result.add(Float.parseFloat(token));

            return result;
        }

        private List<String> parseTokenList(String open, String close) {
            expectToken(open);
            List<String> tokens = new ArrayList<String>();
            String token;
            while (!close.equals(token = scanner.next()))
                tokens.add(token);

            return tokens;
        }

        private void expectToken(String expected) {
            String actual = scanner.next();
            assertToken(expected, actual);
        }

        private void assertToken(String expected, String actual) {
            if (actual.equals(expected))
                return;

            String msg;
            msg = String.format("'%s' expected, '%s' got", expected, actual);
            throw new InputMismatchException(msg);
        }

        public Map<Integer, List<HmmState>> getPhoneStates() {
            return phoneStates;
        }

        public List<DiagGmm> getGaussianMixtures() {
            return mixtures;
        }

        public int getContextWidth() {
            return contextWidth;
        }

        public int getContextPosition() {
            return contextPosition;
        }

        public EventMap getEventMap() {
            return eventMap;
        }
    }

    @S4Component(type = UnitManager.class)
    public final static String PROP_UNIT_MANAGER = "unitManager";

    @S4String(mandatory = true)
    public final static String PROP_LOCATION = "location";

    private static LogMath logMath = LogMath.getInstance();

    private String location;
    private UnitManager unitManager;

    private Pool<Senone> senonePool;
    private HMMManager hmmManager;
    private Properties modelProperties;
    private Map<String, Unit> contextIndependentUnits;

    int leftContextSize;
    int rightContextSize;

    /**
     * Constructs empty object.
     *
     * Does nothing but is required for instantiation from the context object.
     */
    public KaldiLoader() {
    }

    public KaldiLoader(String location, UnitManager unitManager) {
        init(location, unitManager);
    }

    public void init(String location, UnitManager unitManager) {
        this.location = location;
        this.unitManager = unitManager;
    }

    @Override
    public void newProperties(PropertySheet ps) throws PropertyException {
        init(ps.getString(PROP_LOCATION),
             (UnitManager) ps.getComponent(PROP_UNIT_MANAGER));
    }

    /**
     * Loads the acoustic model.
     *
     * @throws IOException if an error occurs while loading the model
     */
    public void load() throws IOException {
        AcousticModelParser parser = new AcousticModelParser(location);
        senonePool = new Pool<Senone>("senones");

        for (DiagGmm gmm : parser.getGaussianMixtures())
            senonePool.put(gmm.getId(), gmm);

        File file = new File(location, "phones.txt");
        InputStream stream = new URL(file.getPath()).openStream();
        Reader reader = new InputStreamReader(stream);
        BufferedReader br = new BufferedReader(reader);
        Map<String, Integer> symbolTable = new HashMap<String, Integer>();
        String line;

        while (null != (line = br.readLine())) {
            String[] fields = line.split(" ");
            if (Character.isAlphabetic(fields[0].charAt(0)))
                symbolTable.put(fields[0], Integer.parseInt(fields[1]));
        }

        hmmManager = new LazyHmmManager(parser.getEventMap(),
                                        senonePool, symbolTable);
        contextIndependentUnits = new HashMap<String, Unit>();

        for (String phone : symbolTable.keySet()) {
            Unit unit = unitManager.getUnit(phone, "SIL".equals(phone));
            contextIndependentUnits.put(unit.getName(), unit);
            // Ensure monophone HMMs are created.
            hmmManager.get(HMMPosition.UNDEFINED, unit);
        }
    }

    private Properties loadProperties() throws IOException {
        String path = "models/acoustic/wsj/feat.params";
        Reader reader = new InputStreamReader(new FileInputStream(path));
        BufferedReader br = new BufferedReader(reader);
        Properties properties = new Properties();
        String line;

        while ((line = br.readLine()) != null) {
            String[] tokens = line.split(" ");
            properties.put(tokens[0], tokens[1]);
        }

        return properties;
    }

    /**
     * Gets the senone pool for this loader.
     *
     * @return the pool
     */
    public Pool<Senone> getSenonePool() {
        return senonePool;
    }

    /**
     * Returns the HMM Manager for this loader.
     *
     * @return the HMM Manager
     */
    public HMMManager getHMMManager() {
        return hmmManager;
    }

    /**
     * Returns the map of context indepent units.
     *
     * The map can be accessed by unit name.
     *
     * @return the map of context independent units
     */
    public Map<String, Unit> getContextIndependentUnits() {
        return contextIndependentUnits;
    }

    /**
     * Returns the size of the left context for context dependent units.
     *
     * @return the left context size
     */
    public int getLeftContextSize() {
        return 1;
    }

    /**
     * Returns the size of the right context for context dependent units.
     *
     * @return the right context size
     */
    public int getRightContextSize() {
        return 1;
    }

    /**
     * Returns the model properties
     */
    public Properties getProperties() {
        return modelProperties;
    }

    /**
     * Logs information about this loader
     */
    public void logInfo() {
    }

    /**
     * Not implemented.
     */
    public Pool<float[]> getMeansPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[][]> getMeansTransformationMatrixPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[]> getMeansTransformationVectorPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[]> getVariancePool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[][]> getVarianceTransformationMatrixPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[]> getVarianceTransformationVectorPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[]> getMixtureWeightPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public Pool<float[][]> getTransitionMatrixPool() {
        return null;
    }

    /**
     * Not implemented.
     */
    public float[][] getTransformMatrix() {
        return null;
    }
}
