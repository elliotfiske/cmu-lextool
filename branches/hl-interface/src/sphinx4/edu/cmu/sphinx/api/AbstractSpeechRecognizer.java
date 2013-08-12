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

import java.net.MalformedURLException;
import java.net.URL;

import java.util.List;

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.linguist.acoustic.AcousticModel;
import edu.cmu.sphinx.linguist.language.ngram.LanguageModel;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.result.WordResult;
import edu.cmu.sphinx.util.props.ConfigurationManager;

import static edu.cmu.sphinx.util.props.ConfigurationManagerUtils.resourceToURL;
import static edu.cmu.sphinx.util.props.ConfigurationManagerUtils.setProperty;

/**
 * Base class for Sphinx4 high-level interfaces.
 */
public abstract class AbstractSpeechRecognizer {

    protected final ConfigurationManager configurationManager;

    protected final Recognizer recognizer;
    protected final AudioFileDataSource dataSource;

    public AbstractSpeechRecognizer() {
        this("resource:/edu/cmu/sphinx/config/default.config.xml");
    }

    public AbstractSpeechRecognizer(String resourcePath) {
        String path = resourcePath;
        URL url = null;
        try {
            url = resourceToURL(path);
        } catch (MalformedURLException e) {
            throw new IllegalStateException(path + " not found", e);
        }

        configurationManager = new ConfigurationManager(url);
        recognizer = (Recognizer) configurationManager.lookup("recognizer");
        dataSource = configurationManager.lookup(AudioFileDataSource.class);
    }

    public void setAcousticModel(URL modelPath) {
        setLocalProperty("wsjLoader->location", modelPath);
    }

    public void setDictionary(URL dictionaryPath) {
        setLocalProperty("dictionary->dictionaryPath", dictionaryPath);
    }

    public void setFiller(URL fillerPath) {
        setLocalProperty("dictionary->fillerPath", fillerPath);
    }

    /**
     * Set search path for grammar files.
     *
     * This will enable fixed grammar and disable language model.
     */
    public void setGrammar(URL grammarPath) {
        setLocalProperty("jsgfGrammar->grammarLocation", grammarPath);
        setGlobalProperty("linguist", "flatLinguist");
    }

    /**
     * Set language model.
     *
     * This will disable fixed grammar.
     */
    public void setLanguageModel(URL modelPath) {
        setLocalProperty("lexTreeLinguist->location", modelPath);
        setGlobalProperty("linguist", "lexTreeLinguist");
    }

    public void setMicrophoneInput() {
        // FIXME: implement
    }

    public void setInputSource(URL path) {
        // FIXME: change FrontEnd
        dataSource.setAudioFile(path, "input");
    }

    protected void setLocalProperty(String name, Object value) {
        setProperty(configurationManager, name, value.toString());
    }

    protected void setGlobalProperty(String name, Object value) {
        configurationManager.setGlobalProperty(name, value.toString());
    }
}
