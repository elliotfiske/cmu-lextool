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


/**
 * Aligns text data and audio speech.
 */
public class SpeechAligner {

    private final Context context;
    private final Recognizer recognizer;
    private final AlignerGrammar grammar;

    /**
     * Constructs new aligner object.
     */
    public SpeechAligner(Configuration configuration) {
        try {
            context = new Context(configuration);
            context.setLocalProperty("decoder->searchManager",
                                        "simpleSearchManager");
            context.setLocalProperty("flatLinguist->grammar", "alignerGrammar");

            recognizer = context.getInstance(Recognizer.class);
            grammar = context.getInstance(AlignerGrammar.class);
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    /**
     * Returns time-aligned list of words.
     *
     * @param path path to the speech source
     * @param text text to align
     * @return     list of recognized words with timestamp
     */
    public List<WordResult> align(URL path, String text) {
        recognizer.allocate();
        grammar.setText(text);
        context.setSpeechSource(path);

        List<WordResult> result = recognizer.recognize().getWords();
        recognizer.deallocate();

        return result;
    }
}
