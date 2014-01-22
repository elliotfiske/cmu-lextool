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

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;

import java.net.MalformedURLException;
import java.net.URL;

import edu.cmu.sphinx.api.AbstractSpeechRecognizer;
import edu.cmu.sphinx.api.Configuration;
import edu.cmu.sphinx.api.Context;
import edu.cmu.sphinx.api.SpeechResult;

import edu.cmu.sphinx.frontend.util.StreamCepstrumSource;

/**
 * Example of using Kaldi's acoustic model.
 */
public class YesNo {

    private static class YesNoRecognizer extends AbstractSpeechRecognizer {

        private static final String CONFIG_PATH =
            "resource:/edu/cmu/sphinx/demo/yesno/config.xml";
        private static final String ACOUSTIC_MODEL_PATH =
            "file:models/acoustic/yesno";
        private static final String DICTIONARY_PATH =
            "models/acoustic/yesno/lexicon.txt";

        public static YesNoRecognizer createRecognizer() throws IOException {
            Configuration config = new Configuration();
            config.setAcousticModelPath(ACOUSTIC_MODEL_PATH);
            config.setDictionaryPath(DICTIONARY_PATH);

            config.setGrammarPath("resource:/edu/cmu/sphinx/demo/yesno");
            config.setGrammarName("yesno");
            config.setUseGrammar(true);

            return new YesNoRecognizer(config);
        }

        private YesNoRecognizer(Configuration configuration)
            throws IOException
        {
            super(new Context(CONFIG_PATH, configuration));
        }

        public void startRecognition(String path)
            throws MalformedURLException, IOException
        {
            recognizer.allocate();
            InputStream stream = new FileInputStream(path);
            Class<StreamCepstrumSource> cls = StreamCepstrumSource.class;
            context.getInstance(cls).setInputStream(stream, false);
        }

        public void stopRecognition() {
            recognizer.deallocate();
        }
    }

    private static final String FEAT_PATH =
        "src/apps/edu/cmu/sphinx/demo/yesno/feats/";

    public static void main(String[] args) throws Exception {
        YesNoRecognizer recognizer = YesNoRecognizer.createRecognizer();
        for (String fileName : new File(FEAT_PATH).list()) {
            recognizer.startRecognition(FEAT_PATH + fileName);
            SpeechResult result;

            while ((result = recognizer.getResult()) != null)
                System.out.format("input: %s\nhypothesis: %s\nconfidence: %g\n",
                                  fileName, result.getUtterance(false),
                                  result.getConfidence());

            recognizer.stopRecognition();
        }
    }
}
