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

import java.io.IOException;
import java.io.Writer;

import java.util.ArrayList;
import java.util.Collection;

import edu.cmu.sphinx.result.ConfidenceResult;
import edu.cmu.sphinx.result.ConfidenceScorer;
import edu.cmu.sphinx.result.Lattice;
import edu.cmu.sphinx.result.LatticeOptimizer;
import edu.cmu.sphinx.result.Nbest;
import edu.cmu.sphinx.result.Path;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.result.WordResult;

import edu.cmu.sphinx.util.LogMath;


/**
 * High-level wrapper for {@link Result} instance.
 */
public final class RecognitionResult {

    private final Result result;
    private final Path hypothesis;
    private final Lattice lattice;

    public RecognitionResult(ConfidenceScorer scorer, Result result) {
        this.result = result;
        lattice = new Lattice(result);
        new LatticeOptimizer(lattice).optimize();
        hypothesis = scorer.score(result).getBestHypothesis();
    }

    public Collection<WordResult> getWords() {
        return result.getWords();
    }

    /**
     * Returns string representaion of the result.
     *
     * @param withFillers should the filler parts be included
     */
    public String getUtterance(boolean withFillers) {
        // TODO: use com.google.common.base.Joiner
        Collection<String> words = new ArrayList<String>();

        for (WordResult word : getWords()) {
            if (!word.isFiller() || withFillers)
                words.add(word.getPronunciation().getWord().toString());
        }


        StringBuilder sb = new StringBuilder();
        int count = 0;
        for (String word : words) {
            sb.append(word);
            if (++count < words.size())
                sb.append(" ");
        }

        return sb.toString();
    }

    /**
     * Returns score for the whole utterance.
     *
     * @return total score of the path of words
     */
    public double getScore() {
        return LogMath.getInstance().logToLinear(
                (float) hypothesis.getScore());
    }

    public double getConfidence() {
        return LogMath.getInstance().logToLinear(
                (float) hypothesis.getConfidence());
    }

    public Collection<String> getNbest(int n) {
        return new Nbest(lattice).getNbest(n);
    }

    public void writeLattice(Writer writer) throws IOException {
        lattice.dumpSlf(writer);
    }
}
