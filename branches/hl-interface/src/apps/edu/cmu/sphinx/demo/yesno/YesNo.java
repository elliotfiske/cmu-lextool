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

package edu.cmu.sphinx.demo.yesno;

import edu.cmu.sphinx.util.LogMath;
import java.net.URL;

import edu.cmu.sphinx.api.Configuration;
import edu.cmu.sphinx.api.SpeechResult;
import edu.cmu.sphinx.api.StreamSpeechRecognizer;


/**
 * Example of using Kaldi's acoustic model.
 */
public class YesNo {

    public static void main(String[] args) throws Exception {
        System.out.println("Loading models...");

        Configuration configuration = new Configuration();

        configuration.setAcousticModelPath("models/acoustic/yesno");
        configuration.setDictionaryPath("models/acoustic/yesno/lexicon.txt");
        configuration.setGrammarPath("resource:/edu/cmu/sphinx/demo/yesno");
        configuration.setGrammarName("yesno");
        configuration.setUseGrammar(true);

        StreamSpeechRecognizer recognizer = 
            new StreamSpeechRecognizer(configuration);
        URL audioUrl = new URL("file:src/apps/edu/cmu/sphinx/demo/yesno/feats/1_0_0_0_1_0_0_1.mfc");
//          URL audioUrl = new URL("file:src/apps/edu/cmu/sphinx/demo/yesno/feats/1_1_1_1_1_1_1_1.mfc");
        recognizer.startRecognition(audioUrl.openStream());

        SpeechResult result;

        while ((result = recognizer.getResult()) != null) {
            System.out.format("hypothesis: %s, confidence: %g\n",
                              result.getUtterance(false),
                              result.getConfidence());

            System.out.println("best 3 hypothesis:");
            
            for (String s : result.getNbest(3))
                System.out.println(s);

            System.out.println("Lattice contains " + result.getLattice().getNodes().size() + " nodes");
        }

        recognizer.stopRecognition();
    }
}
