/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */


package edu.cmu.sphinx.linguist.acoustic.tiedstate;

import edu.cmu.sphinx.util.props.Configurable;
import edu.cmu.sphinx.util.props.S4Double;
import edu.cmu.sphinx.util.props.S4Boolean;

import java.util.Map;
import java.io.IOException;


/** Generic interface for a saver of acoustic models */
public interface Saver extends Configurable {

    @S4Double(defaultValue = Sphinx3Loader.PROP_VARIANCE_FLOOR_DEFAULT)
    public final static String PROP_VARIANCE_FLOOR = "varianceFloor";

    /** Mixture component score floor. */
    @S4Double(defaultValue = Sphinx3Loader.PROP_MC_FLOOR_DEFAULT)
    public final static String PROP_MC_FLOOR = "MixtureComponentScoreFloor";

    /** Mixture weight floor. */
    @S4Double(defaultValue = Sphinx3Loader.PROP_MW_FLOOR_DEFAULT)
    public final static String PROP_MW_FLOOR = "mixtureWeightFloor";

    @S4Boolean(defaultValue = true, isNotDefined = true)
    public final static String PROP_SPARSE_FORM = "sparseForm";

    /**
     * Gets the pool of means for this loader
     *
     * @return the pool
     */
    public Pool getMeansPool();


    /**
     * Gets the pool of means transformation matrices for this loader
     *
     * @return the pool
     */
    public Pool getMeansTransformationMatrixPool();


    /**
     * Gets the pool of means transformation vectors for this loader
     *
     * @return the pool
     */
    public Pool getMeansTransformationVectorPool();


    /*
     * Gets the variance pool
     *
     * @return the pool
     */
    public Pool getVariancePool();


    /**
     * Gets the variance transformation matrix pool
     *
     * @return the pool
     */
    public Pool getVarianceTransformationMatrixPool();


    /*
     * Gets the senone pool for this loader
     *
     * @return the pool
     */
    public Pool getSenonePool();


    /**
     * Returns the HMM Manager for this loader
     *
     * @return the HMM Manager
     */
    public HMMManager getHMMManager();


    /**
     * Returns the map of context indepent units. The map can be accessed by unit name.
     *
     * @return the map of context independent units.
     */
    public Map getContextIndependentUnits();


    /** logs information about this loader */
    public void logInfo();


    /**
     * Returns the size of the left context for context dependent units
     *
     * @return the left context size
     */
    public int getLeftContextSize();


    /**
     * Returns the size of the right context for context dependent units
     *
     * @return the left context size
     */
    public int getRightContextSize();

    
    public void save(String name, boolean b) throws IOException;
}


