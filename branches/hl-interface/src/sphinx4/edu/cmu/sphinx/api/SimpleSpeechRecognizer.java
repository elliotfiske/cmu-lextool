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
import edu.cmu.sphinx.result.Result;

public class SimpleSpeechRecognizer extends AbstractSpeechRecognizer {

    private final JSGFGrammar grammar;
    private final Microphone microphone;

    public SimpleSpeechRecognizer() {
        grammar = configurationManager.lookup(JSGFGrammar.class);
        microphone = configurationManager.lookup(Microphone.class);
    }

    public JSGFGrammar getGrammar() {
        return grammar;
    }

    public void startRecognition() {
        recognizer.allocate();
        microphone.startRecording();
    }

    public void stopRecognition() {
        microphone.stopRecording();
        recognizer.deallocate();
    }

    public Result getResult() {
        return recognizer.recognize();
    }
}
