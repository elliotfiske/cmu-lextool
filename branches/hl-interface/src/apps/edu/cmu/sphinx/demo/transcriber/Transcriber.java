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

import edu.cmu.sphinx.api.SimpleSpeechRecognizer;
import edu.cmu.sphinx.result.Result;

/**
 * A simple example that shows how to transcribe a continuous audio file that
 * has multiple utterances in it.
 */
public class Transcriber {

    private static final String ACOUSTIC_MODEL =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz";
    private static final String DICTIONARY_PATH =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz/dict/cmudict.0.6d";
    private static final String FILLTER_PATH =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz/noisedict";
    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/transcriber/";


    public static void main(String[] args) throws MalformedURLException {
        SimpleSpeechRecognizer recognizer = new SimpleSpeechRecognizer();
        recognizer.setGrammar(GRAMMAR_PATH, "digits");
        recognizer.setAcousticModel(ACOUSTIC_MODEL);
        recognizer.setDictionary(DICTIONARY_PATH);
        recognizer.setFiller(FILLTER_PATH);

        if (args.length > 0)
            recognizer.setResourceInput(new File(args[0]).toURI().toURL());
        else
            recognizer.setResourceInput(
                    Transcriber.class.getResource("10001-90210-01803.wav"));

        Result result;
        while ((result = recognizer.getResult()) != null) {
            String hypothesis = result.getBestResultNoFiller();
            System.out.println(hypothesis);
        }
    }
}
