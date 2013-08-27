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

import java.net.URL;

import java.util.List;

import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;

import edu.cmu.sphinx.recognizer.Recognizer;

import edu.cmu.sphinx.result.WordResult;

import edu.cmu.sphinx.util.props.ConfigurationManager;


public class SpeechAligner {

    private final Configuration config;

    private final Recognizer recognizer;
    private final AlignerGrammar grammar;

    public SpeechAligner(final Configuration config) {
        this.config = config;
        config.setLocalProperty("flatLinguist->grammar", "alignerGrammar");
        config.setLocalProperty("decoder->searchManager",
                                "simpleSearchManager");

        ConfigurationManager cm = config.getConfigurationManager();
        recognizer = (Recognizer) cm.lookup("recognizer");
        grammar = (AlignerGrammar) cm.lookup("alignerGrammar");
    }

    public List<WordResult> align(URL path, String text) {
        recognizer.allocate();
        grammar.setText(text);
        config.setSpeechSource(path);

        List<WordResult> result = recognizer.recognize().getWords();
        recognizer.deallocate();

        return result;
    }
}
