/*
 * Copyright 1999-2013 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 */

package edu.cmu.sphinx.demo.transcriber;

import java.io.File;

import java.net.MalformedURLException;
import java.net.URL;

import edu.cmu.sphinx.api.RecognitionResult;
import edu.cmu.sphinx.api.SpeechRecognizer;

import edu.cmu.sphinx.result.Path;


// TODO: use digits.grxml

/**
 * A simple example that shows how to transcribe a continuous audio file that
 * has multiple utterances in it.
 */
public class Transcriber {

    private static final String ACOUSTIC_MODEL =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz";
    private static final String DICTIONARY_PATH =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz/dict/cmudict.0.6d";
    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/transcriber/";


    public static void main(String[] args) throws MalformedURLException {
        SpeechRecognizer recognizer = new SpeechRecognizer();
        recognizer.setGrammar(GRAMMAR_PATH, "digits");
        // recognizer.setLanguageModel("./models/language/en-us.lm.dmp");
        recognizer.setAcousticModel(ACOUSTIC_MODEL);
        recognizer.setDictionary(DICTIONARY_PATH);

        //if (args.length > 0)
        //    recognizer.setResourceInput(new File(args[0]).toURI().toURL());
        //else
        //    recognizer.setResourceInput(
        //            Transcriber.class.getResource("10001-90210-01803.wav"));
        recognizer.setMicrophoneInput();
        recognizer.startRecognition(true);

        RecognitionResult result;
        while ((result = recognizer.getResult()) != null) {
            System.out.format("hypothesis: %s, confidence: %g\n",
                    result.getBestResult(), result.getConfidence());
            System.out.println("best 5 hypothesis:");
            for (String s : result.getNbest(5))
                System.out.println(s);
        }
    }
}
