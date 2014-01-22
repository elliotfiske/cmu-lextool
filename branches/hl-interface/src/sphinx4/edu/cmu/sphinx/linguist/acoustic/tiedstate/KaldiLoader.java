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

    private static final class AcousticModelParser {

        /**
         * Class that can act as a key to transition state.
         *
         * Represent triple (phone, hmm-state, pdf).
         */
        private static final class TransitionState {

            private final int phoneId;
            private final int hmmState;
            private final int pdfId;

            public TransitionState(int phoneId, int hmmState, int pdfId) {
                this.phoneId = phoneId;
                this.hmmState = hmmState;
                this.pdfId = pdfId;
            }

            @Override
            public boolean equals(Object o) {
                if (!(o instanceof TransitionState))
                    return false;

                TransitionState other = (TransitionState) o;
                return phoneId == other.phoneId &&
                       hmmState == other.hmmState &&
                       pdfId == other.pdfId;
            }

            @Override
            public int hashCode() {
                return (hmmState * 31 + phoneId) * 31 + pdfId;
            }

            @Override
            public String toString() {
                return String.format("TransitionState {%d, %d, %d}",
                                     phoneId, hmmState, pdfId);
            }
        }

        private final Scanner scanner;

        private final Map<Integer, List<List<Integer>>> phoneStates =
            new HashMap<Integer, List<List<Integer>>>();

        private final Pool<Senone> senones = new Pool<Senone>("senones");
        private Map<TransitionState, Integer> transitionIds;
        private List<Float> logProbabilities;

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
                throw e;
            } catch (NoSuchElementException e) {
                throw new InputMismatchException("unexpected end of input");
            }
        }

        private void parseModel() {
            parseTransitionModel();
            expectToken("<DIMENSION>");
            // Skip "DIMENSION" value.
            scanner.next();
            expectToken("<NUMPDFS>");
            int npdf = scanner.nextInt();

            for (int i = 0; i < npdf; ++i)
                parseGmm(i);


        }

        private void parseTree() {
            expectToken("ContextDependency");
            contextWidth = scanner.nextInt();
            contextPosition = scanner.nextInt();
            // expectToken("ToPdf");
            // parseEventMap();
            // expectToken("EndContextDependency");
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
            transitionIds = new HashMap<TransitionState, Integer>();
            int tid = 0;

            for (int i = 0; i < numTriples; ++i) {
                int phoneId = scanner.nextInt();
                int hmmState = scanner.nextInt();
                int pdfId = scanner.nextInt();
                TransitionState tstate;
                tstate = new TransitionState(phoneId, hmmState, pdfId);
                transitionIds.put(tstate, tid);
                tid += phoneStates.get(phoneId).get(hmmState).size();
            }

            expectToken("</Triples>");
            expectToken("<LogProbs>");
            logProbabilities = parseFloatList();
            expectToken("</LogProbs>");
            expectToken("</TransitionModel>");
        }

        private void parseTopology() {
            expectToken("<Topology>");
            String token;

            while ("<TopologyEntry>".equals(token = scanner.next())) {
                assertToken("<TopologyEntry>", token);
                expectToken("<ForPhones>");

                List<Integer> phones = new ArrayList<Integer>();
                while (!"</ForPhones>".equals(token = scanner.next()))
                    phones.add(Integer.parseInt(token));

                List<List<Integer>> states = new ArrayList<List<Integer>>();
                while ("<State>".equals(token = scanner.next())) {
                    // Skip state number.
                    scanner.next();
                    token = scanner.next();

                    if ("<PdfClass>".equals(token)) {
                        List<Integer> transitions = new ArrayList<Integer>();
                        int pdfClass = scanner.nextInt();

                        while ("<Transition>".equals(token = scanner.next())) {
                            transitions.add(scanner.nextInt());
                            // Skip initial probability.
                            scanner.next();
                        }
                        states.add(transitions);
                    }
                    assertToken("</State>", token);
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

            senones.put(i, new DiagGmm(i, gconsts, means, vars));
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

        public Pool<Senone> getSenonePool() {
            return senones;
        }

        public float[][] getTransitionMatrix(int phoneId, int pdfId) {
            List<List<Integer>> states = phoneStates.get(phoneId);
            float[][] tmat = new float[states.size() + 1][states.size()];

            for (int i = 0; i < states.size(); ++i) {
                Arrays.fill(tmat[i], LogMath.LOG_ZERO);
                // List<Integer> transitions = states.get(i);

                // TransitionState tstate;
                // tstate = new TransitionState(phoneId, i, pdfId);
                // System.out.println(tstate);
                // int tid = transitionIds.get(tstate);

                for (int j = 0; j < transitions.size(); ++j) {
                    int tid = pdfId * 2;
                    float prob = logProbabilities.get(tid + 1);
                    tmat[i][j] = logMath.lnToLog(prob);
                }
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

    private AcousticModelParser parser;

    private HMMManager hmmManager;
    private Properties modelProperties;
    private Map<String, Unit> contextIndependentUnits;

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
        parser = new AcousticModelParser(location);
        hmmManager = new HMMManager();
        contextIndependentUnits = new HashMap<String, Unit>();

        File file = new File(location, "phones.txt");
        InputStream stream = new URL(file.getPath()).openStream();
        Reader reader = new InputStreamReader(stream);
        BufferedReader br = new BufferedReader(reader);
        Map<String, Integer> phones = new HashMap<String, Integer>();
        String line;

        while (null != (line = br.readLine())) {
            String[] fields = line.split(" ");
            phones.put(fields[0], Integer.parseInt(fields[1]));
        }

        file = new File(location, "context");
        stream = new URL(file.getPath()).openStream();
        reader = new InputStreamReader(stream);
        br = new BufferedReader(reader);

        while (null != (line = br.readLine())) {
            String[] fields = line.split(" ");
            // pdf-id pdf-class left center right
            // if ("<eps>".equals(fields[0])) continue;
            if (fields.length == 3) {
                // CI-phone: pdf-id pdf-class phone
                Unit unit = unitManager.getUnit(fields[2]);
                contextIndependentUnits.put(unit.getName(), unit);
                int pdfId = Integer.parseInt(fields[0]);
                SenoneSequence seq = getSenoneSequence(new int[] { pdfId });
                int phoneId = phones.get(fields[2]);
                float[][] tmat = parser.getTransitionMatrix(phoneId, pdfId);
                HMMPosition position = HMMPosition.UNDEFINED;
                hmmManager.put(new SenoneHMM(unit, seq, tmat, position));
            } else if (fields.length == 5) {
                // CD-phone: pdf-id pdf-class l-phone c-phone r-phone
            }
        }

        modelProperties = loadProperties("models/acoustic/wsj/feat.params");
    }

    private Properties loadProperties(String path) throws IOException {
        Properties properties = new Properties();
        Reader reader = new InputStreamReader(new FileInputStream(path));
        BufferedReader br = new BufferedReader(reader);
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
        return parser.getSenonePool();
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
        return parser.getContextPosition() - 1;
    }

    /**
     * Returns the size of the right context for context dependent units.
     *
     * @return the right context size
     */
    public int getRightContextSize() {
        return parser.getContextWidth() - parser.getContextPosition() + 1;
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
            senones[i] = parser.getSenonePool().get(ids[i]);

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
