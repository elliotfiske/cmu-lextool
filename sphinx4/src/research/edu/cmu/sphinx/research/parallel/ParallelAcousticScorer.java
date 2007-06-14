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

package edu.cmu.sphinx.research.parallel;

import edu.cmu.sphinx.decoder.scorer.AcousticScorer;
import edu.cmu.sphinx.decoder.scorer.Scoreable;
import edu.cmu.sphinx.frontend.*;
import edu.cmu.sphinx.frontend.util.DataUtil;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;

import java.io.IOException;
import java.util.Iterator;
import java.util.List;


/** A parallel acoustic scorer that is capable of scoring multiple feature streams. */
public class ParallelAcousticScorer implements AcousticScorer {

    private String name;


    /* (non-Javadoc)
     * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
     */
    public void newProperties(PropertySheet ps) throws PropertyException {
    }


    /**
     * Scores the given set of Tokens. All Tokens in the given list are assumed to belong to the same acoustic model.
     *
     * @param scoreableList a list containing StateToken objects to be scored
     * @return the best scoring scorable, or null if there are no more frames to score
     */
    public Scoreable calculateScores(List scoreableList) {

        assert scoreableList.size() > 0;

        try {
            FrontEnd frontEnd = getFrontEnd(scoreableList);
            Data data = frontEnd.getData();

            while (data instanceof Signal) {
                data = frontEnd.getData();
            }

            if (data == null)
                return null;

            if (data instanceof DoubleData)
                data = DataUtil.DoubleData2FloatData((DoubleData) data);

            float logMaxScore = -Float.MAX_VALUE;
            Scoreable bestScoreable = null;
            for (Iterator i = scoreableList.iterator(); i.hasNext();) {
                Scoreable scoreable = (Scoreable) i.next();
                float logScore = scoreable.calculateScore(data, false, 1.0f);
                if (logScore > logMaxScore) {
                    logMaxScore = logScore;
                    bestScoreable = scoreable;
                }
            }
            return bestScoreable;
        } catch (DataProcessingException dpe) {
            dpe.printStackTrace();
            return null;
        }
    }


    /**
     * Returns the acoustic model name of the Tokens in the given list .
     *
     * @return the acoustic model name of the Tokens
     */
    private FrontEnd getFrontEnd(List activeList) {
        if (activeList.size() > 0) {
            Iterator i = activeList.iterator();
            if (i.hasNext()) {
                ParallelToken token = (ParallelToken) i.next();
                return token.getFeatureStream().getFrontEnd();
            }
        }
        return null;
    }


    /*
     * (non-Javadoc)
     *
     * @see edu.cmu.sphinx.util.props.Configurable#getName()
     */
    public String getName() {
        return name;
    }


    /** Allocates resources for this scorer */
    public void allocate() throws IOException {
    }


    /** Deallocates resouces for this scorer */
    public void deallocate() {
    }


    /** starts the scorer */
    public void startRecognition() {
    }


    /** stops the scorer */
    public void stopRecognition() {
    }
}
