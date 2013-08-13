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
 * Base class for Sphinx4 high-level interface implementations.
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
            throw new IllegalStateException(e);
        }

        configurationManager = new ConfigurationManager(url);
        recognizer = (Recognizer) configurationManager.lookup("recognizer");
        dataSource = (AudioFileDataSource) configurationManager.lookup("audioFileDataSource");
    }

    public void setAcousticModel(String modelPath) {
        setLocalProperty("wsjLoader->location", modelPath);
    }

    public void setDictionary(String dictionaryPath) {
        setLocalProperty("dictionary->dictionaryPath", dictionaryPath);
    }

    public void setFiller(String fillerPath) {
        setLocalProperty("dictionary->fillerPath", fillerPath);
    }

    /**
     * Set search path for grammar files.
     *
     * This will enable fixed grammar and disable language model.
     */
    public void setGrammar(String grammarPath, String name) {
        // FIXME: use a single param of type File, cache directory part
        setLocalProperty("jsgfGrammar->grammarLocation", grammarPath);
        setLocalProperty("jsgfGrammar->grammarName", name);
        setLocalProperty("decoder->searchManager", "simpleSearchManager");
    }

    /**
     * Set language model.
     *
     * This will disable fixed grammar.
     */
    public void setLanguageModel(String modelPath) {
        setLocalProperty("trigramModel->location", modelPath);
        setLocalProperty("decoder->searchManager", "wordPruningSearchManager");
    }

    public void setMicrophoneInput() {
        setLocalProperty("threadedScorer->frontend", "liveFrontEnd");
    }

    public void setResourceInput(URL path) {
        dataSource.setAudioFile(path, "input");
        setLocalProperty("threadedScorer->frontend", "batchFrontEnd");
    }

    protected void setLocalProperty(String name, Object value) {
        setProperty(configurationManager, name, value.toString());
    }

    protected void setGlobalProperty(String name, Object value) {
        configurationManager.setGlobalProperty(name, value.toString());
    }
}
