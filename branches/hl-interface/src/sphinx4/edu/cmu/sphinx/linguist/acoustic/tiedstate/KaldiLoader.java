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

import java.io.FileInputStream;
import java.io.IOException;

import java.util.*;

import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;

import edu.cmu.sphinx.linguist.acoustic.Unit;


public class KaldiLoader implements Loader {

    /**
     * Loads the acoustic model.
     *
     * @throws IOException if an error occurs while loading the model
     */
    public void load() throws IOException {
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
        return null;
    }

    /**
     * Returns the HMM Manager for this loader.
     *
     * @return the HMM Manager
     */
    public HMMManager getHMMManager() {
        return null;
    }

    /**
     * Returns the map of context indepent units. The map can be accessed by unit name.
     *
     * @return the map of context independent units
     */
    public Map<String, Unit> getContextIndependentUnits() {
        return null;
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
        return 0;
    }

    /**
     * Returns the size of the right context for context dependent units.
     *
     * @return the left context size
     */
    public int getRightContextSize() {
        return 0;
    }
    
    @Override
    public void newProperties(PropertySheet ps) throws PropertyException {

        /*
        init(ConfigurationManagerUtils.getResource(PROP_LOCATION, ps),
                ps.getString(PROP_MODEL), ps.getString(PROP_DATA_LOCATION),
                (UnitManager) ps.getComponent(PROP_UNIT_MANAGER),
                ps.getFloat(PROP_MC_FLOOR), ps.getFloat(PROP_MW_FLOOR),
                ps.getFloat(PROP_VARIANCE_FLOOR),
                ps.getBoolean(PROP_USE_CD_UNITS), ps.getLogger());
                */
    }

    /**
     * Returns the model properties
     */
    public Properties getProperties() {
        return null;
    }

    private static void assertNextToken(Scanner scanner, String expected) {
        String actual = scanner.next().trim();
        if (!actual.equals(expected)) {
            String msg;
            msg = String.format("'%s' expected, '%s' got", expected, actual);
            throw new IllegalStateException(msg);
        }
    }

    public static void main(String[] args) throws IOException {
        Scanner sc = new Scanner(new FileInputStream(args[0]));
        // Skip transition model.
        while (sc.hasNext() && !sc.next().trim().equals("</TransitionModel>"));
        assertNextToken(sc, "<DIMENSION>");
        int ndim = Integer.parseInt(sc.next());

        assertNextToken(sc, "<NUMPDFS>");
        int npdf = Integer.parseInt(sc.next());

        for (int i = 0; i < npdf; ++i) {
            assertNextToken(sc, "<DiagGMM>");
            assertNextToken(sc, "<GCONSTS>");
            assertNextToken(sc, "[");
            List<Float> gconsts = new ArrayList<Float>();
            String token;
            while (!(token = sc.next().trim()).equals("]"))
                gconsts.add(Float.parseFloat(token));

            assertNextToken(sc, "<WEIGHTS>");
            assertNextToken(sc, "[");
            List<Float> weights = new ArrayList<Float>();
            while (!(token = sc.next().trim()).equals("]"))
                weights.add(Float.parseFloat(token));

            assertNextToken(sc, "<MEANS_INVVARS>");
            assertNextToken(sc, "[");
            List<Float> means = new ArrayList<Float>();
            while (!(token = sc.next().trim()).equals("]"))
                means.add(Float.parseFloat(token));

            assertNextToken(sc, "<INV_VARS>");
            assertNextToken(sc, "[");
            List<Float> vars = new ArrayList<Float>();
            while (!(token = sc.next().trim()).equals("]"))
                vars.add(Float.parseFloat(token));

            assertNextToken(sc, "</DiagGMM>");
        }
    }
}
