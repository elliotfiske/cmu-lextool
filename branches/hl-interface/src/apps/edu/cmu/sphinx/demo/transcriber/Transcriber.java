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
import java.io.PrintWriter;
import java.io.Writer;

import java.net.MalformedURLException;
import java.net.URL;

import edu.cmu.sphinx.api.Configuration;
import edu.cmu.sphinx.api.SpeechResult;
import edu.cmu.sphinx.api.LiveSpeechRecognizer;

import edu.cmu.sphinx.result.Path;


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


    public static void main(String[] args) throws Exception {
        System.out.println("Loading models...");

        Configuration configuration = new Configuration();
        configuration.setAcousticModelPath(ACOUSTIC_MODEL);
        configuration.setDictionaryPath(DICTIONARY_PATH);
        configuration.setLanguageModelPath("./models/language/en-us.lm.dmp");

        LiveSpeechRecognizer recognizer = 
            new LiveSpeechRecognizer(configuration);
        recognizer.startRecognition(true);

        System.out.println("Say something (\"the end\" to exit):");
        Writer writer = new PrintWriter(System.out);
        SpeechResult result;

        while ((result = recognizer.getResult()) != null) {
            String utterance = result.getUtterance(false);
            if (utterance.equals("the end"))
                break;

            System.out.format("hypothesis: %s, confidence: %g\n",
                              utterance, result.getConfidence());

            System.out.println("best 3 hypothesis:");
            for (String s : result.getNbest(3))
                System.out.println(s);

            result.getLattice().dumpSlf(writer);
            writer.flush();
        }

        writer.close();
        recognizer.stopRecognition();
    }
}
