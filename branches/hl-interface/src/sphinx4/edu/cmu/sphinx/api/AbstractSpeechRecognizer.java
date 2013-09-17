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

import edu.cmu.sphinx.result.ConfidenceScorer;
import edu.cmu.sphinx.result.Result;

import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.recognizer.Recognizer.State;


/**
 * Base class for high-level speech recognizers.
 */
public class AbstractSpeechRecognizer {

    protected final Context context;
    protected final Recognizer recognizer;
    protected final ConfidenceScorer scorer;

    protected final SpeechSourceProvider speechSourceProvider;

    /**
     * Constructs recognizer object using provided configuration.
     */
    public AbstractSpeechRecognizer(Configuration configuration) {
        try {
            context = new Context(configuration);
            recognizer = context.getInstance(Recognizer.class);
            scorer = context.getInstance(ConfidenceScorer.class);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        speechSourceProvider = new SpeechSourceProvider();
    }

    /**
     * Returns result of the recognition.
     */
    public SpeechResult getResult() {
        Result result = recognizer.recognize();
        return null == result ? null : new SpeechResult(scorer, result);
    }
}
