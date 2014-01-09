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

    @S4Component(type = UnitManager.class)
    public final static String PROP_UNIT_MANAGER = "unitManager";

    @S4String(mandatory = true)
    public final static String PROP_LOCATION = "location";

    private URL location;
    private HMMManager hmmManager;
    private UnitManager unitManager;

    private Pool<Senone> senonePool;
    private Properties modelProperties;
    private Map<String, Unit> contextIndependentUnits;

    public KaldiLoader() {
    }

    public KaldiLoader(URL location, UnitManager unitManager) {
        init(location, unitManager);
    }

    public void init(URL location, UnitManager unitManager) {
        this.location = location;
        this.unitManager = unitManager;
    }

    @Override
    public void newProperties(PropertySheet ps) throws PropertyException {
        init(ConfigurationManagerUtils.getResource(PROP_LOCATION, ps),
             (UnitManager) ps.getComponent(PROP_UNIT_MANAGER));
    }

    /**
     * Loads the acoustic model.
     *
     * @throws IOException if an error occurs while loading the model
     */
    public void load() throws IOException {
        senonePool = loadSenones();
        hmmManager = new HMMManager();
        contextIndependentUnits = new HashMap<String, Unit>();

        Unit unit = UnitManager.SILENCE;
        contextIndependentUnits.put(unit.getName(), unit);
        SenoneSequence ss = getSenoneSequence(new int[] {0, 1});
        LogMath logMath = LogMath.getInstance();
        float[][] transitionMatrix = {
            {logMath.lnToLog(-0.1219371f), logMath.lnToLog(-2.164599f), 0},
            {0, logMath.lnToLog(-0.04802629f), logMath.lnToLog(-3.059923f)}
            /*
            {0.75f, 0.25f, 0},
            {0, 0.75f, 0.25f}
            */
        };
        hmmManager.put(new SenoneHMM(unit, ss, transitionMatrix,
                                     HMMPosition.UNDEFINED));

        unit = unitManager.getUnit("Y_S", false);
        contextIndependentUnits.put(unit.getName(), unit);
        ss = getSenoneSequence(new int[] {2, 3});
        transitionMatrix = new float[][] {
            {logMath.lnToLog(-0.05272233f), logMath.lnToLog(-2.968962f), 0 },
            {0, logMath.lnToLog(-0.04890276f), logMath.lnToLog(-3.042274f) }
            /*
            {0.75f, 0.25f, 0},
            {0, 0.75f, 0.25f}
            */
        };
        hmmManager.put(new SenoneHMM(unit, ss, transitionMatrix,
                                     HMMPosition.UNDEFINED));

        unit = unitManager.getUnit("N_S", false);
        contextIndependentUnits.put(unit.getName(), unit);
        ss = getSenoneSequence(new int[] {4, 5});
        transitionMatrix = new float[][] {
            {logMath.lnToLog(-0.04344284f), logMath.lnToLog(-3.157953f), 0 },
            {0, logMath.lnToLog(-0.04845489f), logMath.lnToLog(-3.051252f) }
            /*
            {0.75f, 0.25f, 0},
            {0, 0.75f, 0.25f}
            */
        };
        hmmManager.put(new SenoneHMM(unit, ss, transitionMatrix,
                                     HMMPosition.UNDEFINED));
    }

    private Pool<Senone> loadSenones() throws IOException {
        // TODO: use StreamTokenizer as it's faster than Scanner.
        Scanner sc = new Scanner(location.openStream());

        /*
        assertNextToken(sc, "<TransitionModel>");

        assertNextToken(sc, "<Topology>");
        String token;
        while (!(token = sc.nextToken()).equals("</TopologyEntry>")) {
            assertToken("<TopologyEntry>", token);
            assertNextToken(sc, "<ForPhones>");

            List<Integer> phones = new ArrayList<Integer>();
            while (!(token = sc.nextToken()).equals("</ForPhones>"))
                phones.add(Integer.parseInt(token));
        }
        assertNextToken(sc, "</Topology>");

        assertNextToken(sc, "<Triples>");
        int numTriples = Integer.parseInt(sc.nextToken());
        int[][] triples = new int[6][3];
        for (int i = 0; i < numTriples; ++i) {
            for (int j = 0; j < 3; ++j)
                triples[i][j] = Integer.parseInt(sc.nextToken());
        }
        assertNextToken(sc, "</Triples>");

        assertNextToken(sc, "<LogProbs>");
        List<Float> logProbs = parseFloatList(sc);
        assertNextToken(sc, "</LogProbs>");

        assertNextToken(sc, "</TransitionModel>");
        */
        while (sc.hasNext() && !sc.next().trim().equals("</TransitionModel>"));

        assertNextToken(sc, "<DIMENSION>");
        int ndim = Integer.parseInt(sc.next());

        assertNextToken(sc, "<NUMPDFS>");
        int npdf = Integer.parseInt(sc.next());

        Pool<Senone> senones = new Pool<Senone>("senones");

        for (int i = 0; i < npdf; ++i) {
            assertNextToken(sc, "<DiagGMM>");
            assertNextToken(sc, "<GCONSTS>");
            List<Float> gconsts = parseFloatList(sc);

            assertNextToken(sc, "<WEIGHTS>");
            List<Float> weights = parseFloatList(sc);

            assertNextToken(sc, "<MEANS_INVVARS>");
            List<Float> means = parseFloatList(sc);

            assertNextToken(sc, "<INV_VARS>");
            List<Float> vars = parseFloatList(sc);

            senones.put(i, new DiagGmm(i, gconsts, means, vars));

            assertNextToken(sc, "</DiagGMM>");
        }

        return senones;
    }

    private Properties loadProperties(String path)
        throws IOException
    {
        Properties properties = new Properties();
        Reader reader = new InputStreamReader(new FileInputStream(path));
        BufferedReader br = new BufferedReader(reader);
        String line;

        while ((line = br.readLine()) != null) {
            String[] tokens = line.split(" ");
            properties.put(tokens[0].trim(), tokens[1].trim());
        }

        return properties;
    }

    private static void assertNextToken(Scanner scanner, String expected) {
        String actual = scanner.next().trim();
        assertToken(expected, actual);
    }

    private static void assertToken(String expected, String actual) {
        if (actual.equals(expected))
            return;

        String msg;
        msg = String.format("'%s' expected, '%s' got", expected, actual);
        throw new IllegalStateException(msg);
    }

    private static List<Float> parseFloatList(Scanner scanner) {
        assertNextToken(scanner, "[");

        List<Float> result = new ArrayList<Float>();
        String token;

        while (!(token = scanner.next().trim()).equals("]"))
            result.add(Float.parseFloat(token));

        return result;
    }

    /**
     * Gets the pool of means for this loader.
     *
     * @return the pool
     */
    public Pool<float[]> getMeansPool() {
        return null;
    }

    /**
     * Gets the means transformation matrix pool.
     *
     * @return the pool
     */
    public Pool<float[][]> getMeansTransformationMatrixPool() {
        return null;
    }

    /**
     * Gets the means transformation vectors pool.
     *
     * @return the pool
     */
    public Pool<float[]> getMeansTransformationVectorPool() {
        return null;
    }

    /**
     * Gets the variance pool.
     *
     * @return the pool
     */
    public Pool<float[]> getVariancePool() {
        return null;
    }

    /**
     * Gets the variance transformation matrix pool.
     *
     * @return the pool
     */
    public Pool<float[][]> getVarianceTransformationMatrixPool() {
        return null;
    }

    /**
     * Gets the variance transformation vectors pool.
     *
     * @return the pool
     */
    public Pool<float[]> getVarianceTransformationVectorPool() {
        return null;
    }

    /**
     * Gets the mixture weight pool.
     *
     * @return the pool
     */
    public Pool<float[]> getMixtureWeightPool() {
        return null;
    }

    /**
     * Gets the transition matrix pool.
     *
     * @return the pool
     */
    public Pool<float[][]> getTransitionMatrixPool() {
        return null;
    }

    /**
     * Gets the transformation matrix.
     *
     * @return the matrix
     */
    public float[][] getTransformMatrix() {
        return null;
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
     * Returns the map of context indepent units. The map can be accessed by unit name.
     *
     * @return the map of context independent units
     */
    public Map<String, Unit> getContextIndependentUnits() {
        return contextIndependentUnits;
    }

    /** logs information about this loader */
    public void logInfo() {
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
     * @return the left context size
     */
    public int getRightContextSize() {
        return 1;
    }
    
    /**
     * Returns the model properties
     */
    public Properties getProperties() {
        try {
            if (null == modelProperties)
                modelProperties = loadProperties("models/acoustic/wsj/feat.params");
        } catch (Exception e) {
            e.printStackTrace();
        }

        return modelProperties;
    }

    protected SenoneSequence getSenoneSequence(int[] ids) {
        Senone[] senones = new Senone[ids.length];
        for (int i = 0; i < ids.length; i++)
            senones[i] = senonePool.get(ids[i]);

        return new SenoneSequence(senones);
    }
}
