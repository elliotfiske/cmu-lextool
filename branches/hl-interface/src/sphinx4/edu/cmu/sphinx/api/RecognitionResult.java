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

package edu.cmu.sphinx.api;

import java.util.Collection;

import edu.cmu.sphinx.result.ConfidenceResult;
import edu.cmu.sphinx.result.ConfidenceScorer;
import edu.cmu.sphinx.result.Lattice;
import edu.cmu.sphinx.result.LatticeOptimizer;
import edu.cmu.sphinx.result.Nbest;
import edu.cmu.sphinx.result.Path;
import edu.cmu.sphinx.result.Result;


/**
 * High-level wrapper for {@link Result} instance.
 */
public class RecognitionResult {

    private final Result result;
    private final Path hypothesis;

    public RecognitionResult(ConfidenceScorer scorer, Result result) {
        this.result = result;
        hypothesis = scorer.score(result).getBestHypothesis();
    }

    public String getBestResult() {
        return result.getBestResultNoFiller();
    }

    public String getBestFinalResult() {
        return result.getBestFinalResultNoFiller();
    }

    public double getConfidence() {
        return hypothesis.getLogMath().logToLinear(
                (float) hypothesis.getConfidence());
    }

    public Collection<String> getNbest(int n) {
        Lattice lattice = new Lattice(result);
        new LatticeOptimizer(lattice).optimize();
        return new Nbest(lattice).getNbest(n);
    }
}
