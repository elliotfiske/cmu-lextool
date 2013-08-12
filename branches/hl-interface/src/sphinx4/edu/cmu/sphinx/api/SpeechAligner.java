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
import edu.cmu.sphinx.result.WordResult;

public class SpeechAligner extends AbstractSpeechRecognizer {

    private final AlignerGrammar grammar;

    public SpeechAligner() {
        setLocalProperty("flatLinguist->grammar", "alignerGrammar");
        grammar = configurationManager.lookup(AlignerGrammar.class);
    }

    public List<WordResult> align(URL path, String text) {
        recognizer.allocate();
        grammar.setText(text);
        setResourceInput(path);
        List<WordResult> result = recognizer.recognize().getWords();
        recognizer.deallocate();
        return result;
    }
}
