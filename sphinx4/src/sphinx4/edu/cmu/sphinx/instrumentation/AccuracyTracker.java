/*
 * 
 * Copyright 1999-2004 Carnegie Mellon University.  
 * Portions Copyright 2004 Sun Microsystems, Inc.  
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.instrumentation;

import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.recognizer.Recognizer.State;
import edu.cmu.sphinx.recognizer.StateListener;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.decoder.ResultListener;
import edu.cmu.sphinx.util.NISTAlign;
import edu.cmu.sphinx.util.props.*;

/** Tracks and reports recognition accuracy */
abstract public class AccuracyTracker
        implements
        ResultListener,
        Resetable,
        StateListener,
        Monitor {

    /** A Sphinx property that defines which recognizer to monitor */
    @S4Component(type = Recognizer.class)
    public final static String PROP_RECOGNIZER = "recognizer";

    /** A sphinx property that define whether summary accuracy information is displayed */
    @S4Boolean(defaultValue = true)
    public final static String PROP_SHOW_SUMMARY = "showSummary";

    /** A sphinx property that define whether detailed accuracy information is displayed */
    @S4Boolean(defaultValue = true)
    public final static String PROP_SHOW_DETAILS = "showDetails";

    /** A sphinx property that define whether recognition results should be displayed. */
    @S4Boolean(defaultValue = true)
    public final static String PROP_SHOW_RESULTS = "showResults";


    /** A sphinx property that define whether recognition results should be displayed. */
    @S4Boolean(defaultValue = true)
    public final static String PROP_SHOW_ALIGNED_RESULTS = "showAlignedResults";

    /** A sphinx property that define whether recognition results should be displayed. */
    @S4Boolean(defaultValue = true)
    public final static String PROP_SHOW_RAW_RESULTS = "showRawResults";

    // ------------------------------
    // Configuration data
    // ------------------------------
    private String name;
    private Recognizer recognizer;
    private boolean showSummary;
    private boolean showDetails;
    private boolean showResults;
    private boolean showAlignedResults;
    private boolean showRaw;

    private NISTAlign aligner = new NISTAlign(false, false);


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
    */
    public void newProperties(PropertySheet ps) throws PropertyException {
        Recognizer newRecognizer = (Recognizer) ps.getComponent(PROP_RECOGNIZER);

        if (recognizer == null) {
            recognizer = newRecognizer;
            recognizer.addResultListener(this);
            recognizer.addStateListener(this);
        } else if (recognizer != newRecognizer) {
            recognizer.removeResultListener(this);
            recognizer.removeStateListener(this);
            recognizer = newRecognizer;
            recognizer.addResultListener(this);
            recognizer.addStateListener(this);
        }

        showSummary = ps.getBoolean(PROP_SHOW_SUMMARY);
        showDetails = ps.getBoolean(PROP_SHOW_DETAILS);
        showResults = ps.getBoolean(PROP_SHOW_RESULTS);
        showAlignedResults = ps.getBoolean(PROP_SHOW_ALIGNED_RESULTS);

        showRaw = ps.getBoolean(PROP_SHOW_RAW_RESULTS);

        aligner.setShowResults(showResults);
        aligner.setShowAlignedResults(showAlignedResults);
    }


    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.instrumentation.Resetable
     */
    public void reset() {
        aligner.resetTotals();
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.util.props.Configurable#getName()
    */
    public String getName() {
        return name;
    }


    /**
     * Retrieves the aligner used to track the accuracy stats
     *
     * @return the aligner
     */
    public NISTAlign getAligner() {
        return aligner;
    }


    /**
     * Shows the complete details.
     *
     * @param rawText the RAW result
     */
    protected void showDetails(String rawText) {
        if (showDetails) {
            System.out.println();
            aligner.printSentenceSummary();
            if (showRaw) {
                System.out.println("RAW     " + rawText);
            }
            System.out.println();
            aligner.printTotalSummary();
        }
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.decoder.ResultListener#newResult(edu.cmu.sphinx.result.Result)
    */
    abstract public void newResult(Result result);

    @Override
    public void statusChanged(Recognizer.State status) {
        if (status == State.DEALLOCATED) {
            if (showSummary) {
                System.out.println("\n# --------------- Summary statistics ---------");
                aligner.printTotalSummary();
            }
        }
    }
}
