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
import edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi.*;

import edu.cmu.sphinx.util.props.*;
import edu.cmu.sphinx.util.ExtendedStreamTokenizer;
import edu.cmu.sphinx.util.LogMath;


class XXX {

    private final int phone;
    private final int left;
    private final int right;

    public XXX(int phone) {
        this(phone, -1, -1);
    }

    public XXX(int phone, int left, int right) {
        this.phone = phone;
        this.left = left;
        this.right = right;
    }

    public int getPhone() {
        return phone;
    }

    public int getLeftPhone() {
        return left;
    }

    public int getRightPhone() {
        return right;
    }

    public boolean hasContext() {
        return -1 != left && -1 != right;
    }

    @Override
    public boolean equals(Object object) {
        if (!(object instanceof XXX))
            return false;

        XXX other = (XXX) object;
        return phone == other.phone &&
            left == other.left &&
            right == other.right;
    }

    @Override
    public int hashCode() {
        return 31 * (31 * phone + left) + right;
    }

    @Override
    public String toString() {
        return String.format("Context {%d, %d, %d}", phone, left, right);
    }
}

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

    final class AcousticModelParser {

        private final Scanner scanner;

        // phone -> topology
        private Map<Integer, List<HmmState>> phoneStates;
        private Map<Triple, Integer> transitionStates;
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
            expectToken("ToPdf");
            EventMap eventMap = parseEventMap();
        }

        private EventMap parseEventMap() {
            String token = scanner.next();
            if ("NULL".equals(token))
                return null;

            if ("CE".equals(token))
                return parseConstantTableEventMap();
            else if ("SE".equals(token))
                return parseSplitEventMap();
            else if ("TE".equals(token))
                return parseTableEventMap();
            else
                throw new InputMismatchException(token);
        }

        private EventMap parseConstantTableEventMap() {
            return new ConstantEventMap(scanner.nextInt());
        }

        private EventMap parseSplitEventMap() {
            int key = scanner.nextInt();
            Collection<Integer> answers = new ArrayList<Integer>();
            for (String token : parseTokenList("[", "]"))
                answers.add(Integer.valueOf(token));

            expectToken("{");
            EventMap eventMap = new SplitEventMap(key, answers,
                                                  parseEventMap(),
                                                  parseEventMap());
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
    }

    @S4Component(type = UnitManager.class)
    public final static String PROP_UNIT_MANAGER = "unitManager";

    @S4String(mandatory = true)
    public final static String PROP_LOCATION = "location";

    private static LogMath logMath = LogMath.getInstance();

    private String location;
    private UnitManager unitManager;

    private Map<Integer, String> phones;
    private Pool<Senone> senonePool;
    private HMMManager hmmManager;
    private Properties modelProperties;
    private Map<String, Unit> contextIndependentUnits;
    // context -> [state -> pdf]
    private Map<XXX, SortedMap<Integer, Integer>> contextData;

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

        for (DiagGmm gmm : parser.getGaussianMixtures())
            senonePool.put(gmm.getId(), gmm);

        URL mdefUrl = new URL(new File(location, "mdef").getPath());
        Scanner sc = new Scanner(mdefUrl.openStream());

        contextIndependentUnits = new HashMap<String, Unit>();
        hmmManager = new HMMManager();
        int[] gmms = new int[3];

        while (sc.hasNext()) {
            String phone = sc.next();
            String left = sc.next();
            String right = sc.next();
            String position = sc.next();

            gmms[0] = sc.nextInt();
            gmms[1] = sc.nextInt();
            gmms[2] = sc.nextInt();

            float[][] tmat = new float[4][4];
            Arrays.fill(tmat[3], LogMath.LOG_ZERO);
            for (int i = 0; i < 3; ++i) {
                Arrays.fill(tmat[i], LogMath.LOG_ZERO);
                tmat[i][i] = logMath.lnToLog(sc.nextFloat());
                tmat[i][i + 1] = logMath.lnToLog(sc.nextFloat());
            }

            Unit unit;
            Unit prevUnit = null;
            String prevUnitName = null;

            if ("-".equals(left)) {
                unit = unitManager.getUnit(phone, "SIL".equals(phone));
                contextIndependentUnits.put(unit.getName(), unit);
            } else {
                String unitName = (phone + ' ' + left + ' ' + right);

                if (unitName.equals(prevUnitName)) {
                    unit = prevUnit;
                } else {
                    Unit[] leftContext = new Unit[1];
                    leftContext[0] = contextIndependentUnits.get(left);
                    Unit[] rightContext = new Unit[1];
                    rightContext[0] = contextIndependentUnits.get(right);
                    Context context;
                    context = LeftRightContext.get(leftContext, rightContext);
                    unit = unitManager.getUnit(phone, false, context);
                }

                prevUnitName = unitName;
                prevUnit = unit;
            }

            SenoneSequence ss = getSenoneSequence(gmms);
            HMMPosition pos = HMMPosition.lookup(position);
            hmmManager.put(new SenoneHMM(unit, ss, tmat, pos));
        }

        if (hmmManager.get(HMMPosition.UNDEFINED, UnitManager.SILENCE) == null)
            throw new IOException("Could not find SIL unit in acoustic model");
    }

    private void loadPhones() throws IOException {
        // Load base phones.
        File file = new File(location, "phones.txt");
        InputStream stream = new URL(file.getPath()).openStream();
        Reader reader = new InputStreamReader(stream);
        BufferedReader br = new BufferedReader(reader);
        phones = new HashMap<Integer, String>();
        String line;

        while (null != (line = br.readLine())) {
            // Line format: <PHONE> <PHONE-ID>.
            String[] fields = line.split(" ");
            phones.put(Integer.parseInt(fields[1]), fields[0]);
        }
    }

    private void loadContext(AcousticModelParser parser) throws IOException {
        hmmManager = new HMMManager();
        contextData = new HashMap<XXX, SortedMap<Integer, Integer>>();

        File file = new File(location, "context");
        InputStream stream = new URL(file.getPath()).openStream();
        Reader reader = new InputStreamReader(stream);
        BufferedReader br = new BufferedReader(reader);
        String line;

        while (null != (line = br.readLine())) {
            int phone;
            XXX context;
            String[] fields = line.split(" ");

            if (fields.length > 3) {
                phone = Integer.parseInt(fields[3]);
                context = new XXX(phone,
                                      Integer.parseInt(fields[2]),
                                      Integer.parseInt(fields[4]));
            } else {
                phone = Integer.parseInt(fields[2]);
                context = new XXX(phone);
            }

            SortedMap<Integer, Integer> states;
            if (!contextData.containsKey(context))
                contextData.put(context,
                        states = new TreeMap<Integer, Integer>());
            else
                states = contextData.get(context);

            int pdf = Integer.parseInt(fields[0]);
            int pdfClass = Integer.parseInt(fields[1]);

            // For each state with <pdf-class> within HMM topology for the
            // <phone> assign <pdf-id>.
            for (HmmState state : parser.getPhoneStates().get(phone)) {
                if (pdfClass == state.getPdfClass())
                    states.put(state.getId(), pdf);
            }
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

    public float[][] getTransitionMatrix(
            AcousticModelParser parser, XXX context)
    {
        List<HmmState> states = parser.getPhoneStates().get(context.getPhone());
        float[][] tmat = new float[states.size() + 1][states.size() + 1];

        for (Integer i : contextData.get(context).keySet()) {
            Arrays.fill(tmat[i], LogMath.LOG_ZERO);
            int pdf = contextData.get(context).get(i);
            Triple triple = new Triple(context.getPhone(), i, pdf);
            int idx = parser.getTransitionIndex(triple);
            int tid = 0;

            for (Integer j : states.get(i).getTransitions())
                tmat[i][j] = parser.getProbability(idx + tid++);
        }

        Arrays.fill(tmat[states.size()], LogMath.LOG_ZERO);

        return tmat;
    }

    protected SenoneSequence getSenoneSequence(int[] stateid) {
        Senone[] senones = new Senone[stateid.length];
        for (int i = 0; i < stateid.length; i++) {
            senones[i] = senonePool.get(stateid[i]);
        }
        return new SenoneSequence(senones);
    }


    private SenoneSequence getSenoneSequence(XXX context) {
        Map<Integer, Integer> states = contextData.get(context);
        Senone[] senones = new Senone[states.size()];
        for (Map.Entry<Integer, Integer> e : states.entrySet())
            senones[e.getKey()] = senonePool.get(e.getValue());

        return new SenoneSequence(senones);
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
