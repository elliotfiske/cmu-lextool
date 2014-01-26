/*
 * Copyright 2013 Carnegie Mellon University.  
 * Portions Copyright 2004 Sun Microsystems, Inc.  
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.linguist.acoustic.tiedstate;

import java.io.*;
import java.net.*;
import java.util.*;

import edu.cmu.sphinx.linguist.acoustic.*;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi.DiagGmm;

import edu.cmu.sphinx.util.props.*;
import edu.cmu.sphinx.util.LogMath;


public class KaldiLoader implements Loader {

    final class AcousticModelParser {

        private final class HmmState {

            private final int pdfClass;
            private final List<Integer> transitions;

            public HmmState() {
                transitions = new ArrayList<Integer>();
                pdfClass = scanner.nextInt();
                String token;

                while ("<Transition>".equals(token = scanner.next())) {
                    transitions.add(scanner.nextInt());
                    // Skip initial probability.
                    scanner.next();
                }

                assertToken("</State>", token);
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
        }

        private final Scanner scanner;

        // phone -> topology
        private Map<Integer, List<HmmState>> phoneStates;
        private List<Float> logProbabilities;
        private List<DiagGmm> mixtures;

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
        }

        private void parseEventMap() {
            String token = scanner.next();
            if ("NULL".equals(token)) return;

            if ("CE".equals(token))
                parseConstantTableEventMap();
            else if ("TE".equals(token))
                parseTableEventMap();
            else if ("SE".equals(token))
                parseSplitEventMap();
            else
                throw new InputMismatchException(token);
        }

        private void parseConstantTableEventMap() {
            int pdfId = scanner.nextInt();
        }

        private void parseTableEventMap() {
            int pos = scanner.nextInt();
            int tableSize = scanner.nextInt();
            expectToken("(");
            while (0 < tableSize--)
                parseEventMap();
            expectToken(")");
        }

        private void parseSplitEventMap() {
            int pos = scanner.nextInt();
            expectToken("[");
            String token;
            while (!"]".equals(token = scanner.next()));
            expectToken("{");
            parseEventMap();
            parseEventMap();
            expectToken("}");
        }

        private void parseTransitionModel() {
            expectToken("<TransitionModel>");
            parseTopology();

            expectToken("<Triples>");
            int numTriples = scanner.nextInt();
            int transitionId = 1;

            for (int i = 0; i < numTriples; ++i) {
                int phoneId = scanner.nextInt();
                List<HmmState> states = phoneStates.get(phoneId);
                int stateIndex = scanner.nextInt();
                HmmState hmmState = states.get(stateIndex);
               //  float[][] tmat = transitionMatrices.get(phoneId);
                for (Integer transitionIndex : hmmState.getTransitions());
                    // tmat[i][] = probabilities.get(transitionId++);
            }

            expectToken("</Triples>");
            expectToken("<LogProbs>");
            logProbabilities = parseFloatList();
            expectToken("</LogProbs>");
            expectToken("</TransitionModel>");
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
                    scanner.next();
                    token = scanner.next();

                    if ("<PdfClass>".equals(token))
                        states.add(new HmmState());
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
            if (actual.equals(expected)) return;

            String msg;
            msg = String.format("'%s' expected, '%s' got", expected, actual);
            throw new InputMismatchException(msg);
        }

        public List<DiagGmm> getGaussianMixtures() {
            return mixtures;
        }

        public float[][] getTransitionMatrix(int phoneId, int pdfId) {
            List<HmmState> states = phoneStates.get(phoneId);
            float[][] tmat = new float[states.size() + 1][states.size()];

            for (int i = 0; i < states.size(); ++i) {
                Arrays.fill(tmat[i], LogMath.LOG_ZERO);
                // List<Integer> transitions = states.get(i);

                // TransitionState tstate;
                // tstate = new TransitionState(phoneId, i, pdfId);
                // System.out.println(tstate);
                // int tid = transitionIds.get(tstate);

                //for (int j = 0; j < transitions.size(); ++j) {
                //    int tid = pdfId * 2;
                //    float prob = logProbabilities.get(tid + 1);
                //    tmat[i][j] = logMath.lnToLog(prob);
                //}
            }

            Arrays.fill(tmat[states.size()], LogMath.LOG_ZERO);
            return tmat;
        }

        public int[] getPhonePdfs(int phoneId) {
            return null;
        }

        public int getContextWidth() {
            return contextWidth;
        }

        public int getContextPosition() {
            return contextPosition;
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
     * Empty consructor.
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
        leftContextSize = parser.getContextPosition();
        rightContextSize = parser.getContextWidth() - leftContextSize - 1;

        for (DiagGmm gmm : parser.getGaussianMixtures())
            senonePool.put(0, gmm);

        loadPhones();
        loadContext(parser);
        loadProperties();
    }

    private void loadPhones() throws IOException {
        // Load base phones.
        File file = new File(location, "phones.txt");
        InputStream stream = new URL(file.getPath()).openStream();
        Reader reader = new InputStreamReader(stream);
        BufferedReader br = new BufferedReader(reader);
        Map<String, Integer> phones = new HashMap<String, Integer>();
        contextIndependentUnits = new HashMap<String, Unit>();
        String line;

        while (null != (line = br.readLine())) {
            // Line format: <PHONE> <PHONE-ID>.
            String[] fields = line.split(" ");
            phones.put(fields[0], Integer.parseInt(fields[1]));
            // TODO: set valid silence flag
            Unit unit = unitManager.getUnit(fields[0], false);
            contextIndependentUnits.put(unit.getName(), unit);

            // int pdfId = Integer.parseInt(fields[0]);
            // SenoneSequence seq = getSenoneSequence(new int[] { pdfId });
            // int phoneId = phones.get(fields[2]);
            // float[][] tmat = parser.getTransitionMatrix(phoneId, pdfId);
            // HMMPosition position = HMMPosition.UNDEFINED;
            // hmmManager.put(new SenoneHMM(unit, seq, tmat, position));
        }
    }

    private void loadContext(AcousticModelParser parser) throws IOException {
        hmmManager = new HMMManager();

        File file = new File(location, "context");
        InputStream stream = new URL(file.getPath()).openStream();
        Reader reader = new InputStreamReader(stream);
        BufferedReader br = new BufferedReader(reader);
        String line;

        while (null != (line = br.readLine())) {
            String[] fields = line.split(" ");
            // Skip short records as they are probably for silence phones.
            if (fields.length < 5) continue;

            int phoneId = Integer.parseInt(fields[4]);
            int left = Integer.parseInt(fields[3]);
            int right = Integer.parseInt(fields[5]);
            //Context context = new Context(phoneId, left, right);
            //if (!states.containsKey(context)) {
            //    states.put(context, new SortedSet<Index>(index));
            //}
            // Line format: <PDF-ID> <PDF-CLASS> <LEFT> <CENTER> <RIGHT>
            //SenoneSequence seq = getSenoneSequence(new int[] { pdfId });
            //int phoneId = phones.get(fields[2]);
            //float[][] tmat = parser.getTransitionMatrix(phoneId, pdfId);
            //HMMPosition position = HMMPosition.UNDEFINED;
            //Unit unit = unitManager.getUnit(fields[0], false);
            //hmmManager.put(new SenoneHMM(unit, seq, tmat, position));
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
        return leftContextSize;
    }

    /**
     * Returns the size of the right context for context dependent units.
     *
     * @return the right context size
     */
    public int getRightContextSize() {
        return rightContextSize;
    }

    /**
     * Returns the model properties
     */
    public Properties getProperties() {
        return modelProperties;
    }

    protected SenoneSequence getSenoneSequence(int[] ids) {
        Senone[] senones = new Senone[ids.length];
        for (int i = 0; i < ids.length; i++)
            senones[i] = senonePool.get(ids[i]);

        return new SenoneSequence(senones);
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
