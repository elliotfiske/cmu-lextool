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

import java.net.URL;

import java.util.List;

import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;

import edu.cmu.sphinx.recognizer.Recognizer;

import edu.cmu.sphinx.result.WordResult;

import edu.cmu.sphinx.util.props.ConfigurationManager;


public class SpeechAligner {

    private final Configurer configurer;
    private final Recognizer recognizer;
    private final AlignerGrammar grammar;

    public SpeechAligner(Configuration configuration) {
        try {
            configurer = new Configurer(configuration);
            configurer.setLocalProperty("decoder->searchManager",
                                        "simpleSearchManager");
            configurer.setLocalProperty("flatLinguist->grammar", "alignerGrammar");

            recognizer = configurer.getInstance(Recognizer.class);
            grammar = configurer.getInstance(AlignerGrammar.class);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    public List<WordResult> align(URL path, String text) {
        recognizer.allocate();
        grammar.setText(text);
        configurer.setSpeechSource(path);

        List<WordResult> result = recognizer.recognize().getWords();
        recognizer.deallocate();

        return result;
    }
}
