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

import edu.cmu.sphinx.frontend.util.Microphone;

import edu.cmu.sphinx.jsgf.JSGFGrammar;

import edu.cmu.sphinx.result.ConfidenceScorer;
import edu.cmu.sphinx.result.Result;

import edu.cmu.sphinx.recognizer.Recognizer.State;


public class SimpleSpeechRecognizer extends AbstractSpeechRecognizer {

    private final JSGFGrammar grammar;
    private final Microphone microphone;

    public SimpleSpeechRecognizer() {
        grammar = configurationManager.lookup(JSGFGrammar.class);
        microphone = configurationManager.lookup(Microphone.class);
    }

    // FIXME: documentation
    /**
     * Starts recognition process.
     *
     * @param clear Should we continue the current session or start new
     * recognition. 
     */
    public void startRecognition(boolean clear) {
        recognizer.allocate();

        if (clear)
            microphone.clear();
        microphone.startRecording();
    }

    /**
     * Stops recognition process.
     *
     * Recognition process is paused until the next call to startRecognition.
     *
     * @see startRecognition
     */
    public void stopRecognition() {
        microphone.stopRecording();
        recognizer.deallocate();
    }

    public RecognitionResult getResult() {
        if (State.DEALLOCATED == recognizer.getState())
            recognizer.allocate();

        Result result = recognizer.recognize();
        if (null == result)
            return null;

        ConfidenceScorer scorer =
            (ConfidenceScorer) configurationManager.lookup("confidenceScorer");
        return new RecognitionResult(scorer, result);
    }
}
