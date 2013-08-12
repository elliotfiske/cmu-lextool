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

package edu.cmu.sphinx.demo.dialog;

import java.io.IOException;

import java.net.MalformedURLException;
import java.net.URL;

import edu.cmu.sphinx.api.SimpleSpeechRecognizer;
import edu.cmu.sphinx.jsgf.JSGFGrammarException;
import edu.cmu.sphinx.jsgf.JSGFGrammarParseException;
import edu.cmu.sphinx.result.Result;

public class Dialog {

    private static final String ACOUSTIC_MODEL =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz";
    private static final String DICTIONARY_PATH =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz/dict/cmudict.0.6d";
    private static final String FILLTER_PATH =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz/noisedict";
    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/dialog/";
    private static final String LANGUAGE_MODEL =
        "resource:/edu/cmu/sphinx/demo/jsapi/dialog/weather.lm";

    public static void main(String[] args)
        throws IOException, MalformedURLException, JSGFGrammarException, JSGFGrammarParseException
    {
        SimpleSpeechRecognizer recognizer = new SimpleSpeechRecognizer();
        recognizer.setAcousticModel(ACOUSTIC_MODEL);
        recognizer.setGrammar(GRAMMAR_PATH, "menu");
        recognizer.setDictionary(DICTIONARY_PATH);
        recognizer.setFiller(FILLTER_PATH);

        recognizer.startRecognition();
        recognizer.stopRecognition();

        recognizer.getGrammar().loadJSGF("commands");
        recognizer.startRecognition();
        recognizer.stopRecognition();

        recognizer.setLanguageModel(LANGUAGE_MODEL);
        recognizer.startRecognition();
        System.out.println("Start recognition");
        Result result = recognizer.getResult();
        System.out.println(result.getBestResultNoFiller());
        System.out.println("Stop recognition");
        recognizer.stopRecognition();
    }
}
