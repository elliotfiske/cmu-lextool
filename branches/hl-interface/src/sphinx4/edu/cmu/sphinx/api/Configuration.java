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

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;

import edu.cmu.sphinx.util.props.ConfigurationManager;

import static edu.cmu.sphinx.util.props.ConfigurationManagerUtils.resourceToURL;
import static edu.cmu.sphinx.util.props.ConfigurationManagerUtils.setProperty;


/**
 * Helps to tweak configuration without touching XML-file directly.
 */
public class Configuration {

    private final AudioFileDataSource dataSource;
    private final ConfigurationManager configurationManager;

    /**
     * Constructs builder that uses default configuration.
     *
     * @throws MalformedURLException if failed to load configuration file
     */
    public Configuration() throws MalformedURLException {
        this("resource:/edu/cmu/sphinx/config/default.config.xml");
    }

    /**
     * Constructs builder using user-supplied configuration.
     *
     * @param  path path to XML-resource with configuration
     * @return      the same instance of {@link Configuration}
     *
     * @throws MalformedURLException if failed to load configuration file
     */
    public Configuration(String path) throws MalformedURLException {
        URL url = resourceToURL(path);
        configurationManager = new ConfigurationManager(url);
        dataSource = (AudioFileDataSource)
                     configurationManager.lookup("audioFileDataSource");
    }

    /**
     * Sets acoustic model location.
     *
     * @param  path path to directory with acoustic model files
     * @return      the same instance of {@link Configuration}
     */
    public Configuration setAcousticModel(String path) {
        setLocalProperty("wsjLoader->location", path);
        setLocalProperty("dictionary->fillerPath", path + "/noisedict");
        return this;
    }

    /**
     * Sets dictionary.
     *
     * @param path path to directory with dictionary files
     */
    public Configuration setDictionary(String path) {
        setLocalProperty("dictionary->dictionaryPath", path);
        return this;
    }

    /**
     * Sets path to the grammar files.
     *
     * Enables static grammar and disables probabilistic language model.
     * JSGF and GrXML formats are supported.
     *
     * @param path path to the grammar files
     * @param name name of the main grammar to use
     * @return     the same instance of {@link Configuration}
     * @see        Configuration#setLanguageModel(String)
     */
    public Configuration setGrammar(String path, String name) {
        // TODO: use a single param of type File, cache directory part
        if (name.endsWith(".grxml")) {
            setLocalProperty("grXmlGrammar->grammarLocation", path + name);
            setLocalProperty("flatLinguist->grammar", "grXmlGrammar");
        } else {
            setLocalProperty("jsgfGrammar->grammarLocation", path);
            setLocalProperty("jsgfGrammar->grammarName", name);
            setLocalProperty("flatLinguist->grammar", "jsgfGrammar");
            setLocalProperty("decoder->searchManager", "simpleSearchManager");
        }

        return this;
    }

    /**
     * Sets path to the language model.
     *
     * Enables probabilistic language model and distables static grammar.
     * Currently it supports ".lm" and ".dmp" file formats.
     *
     * @param  path path to the language model file
     * @return      the same instance of {@link Configuration}
     * @see         Configuration#setGrammar(String)
     *
     * @throws IllegalArgumentException if path ends with unsupported extension
     */
    public Configuration setLanguageModel(String path) {
        if (path.endsWith(".lm")) {
            setLocalProperty("simpleNGramModel->location", path);
            setLocalProperty(
                "lexTreeLinguist->languageModel", "simpleNGramModel");
        } else if (path.endsWith(".dmp")) {
            setLocalProperty("largeTrigramModel->location", path);
            setLocalProperty(
                "lexTreeLinguist->languageModel", "largeTrigramModel");
        } else {
            throw new IllegalArgumentException(
                "Unknown format extension: " + path);
        }
        setLocalProperty("decoder->searchManager", "wordPruningSearchManager");

        return this;
    }

    /**
     * Sets file or classpath resource as the speech source.
     *
     * @param  url URL of the audio resource
     * @return     the same instance of {@link Configuration}
     * @see        Configuration#useMicrophone()
     */
    public Configuration setSpeechSource(URL url) {
        dataSource.setAudioFile(url, "input");
        setLocalProperty("threadedScorer->frontend", "batchFrontEnd");
        return this;
    }

    /**
     * Sets microphone as the speech source.
     *
     * @return the same instance of {@link Configuration}
     * @see    Configuration#setSpeechSource(URL)
     */
    public Configuration useMicrophone() {
        setLocalProperty("threadedScorer->frontend", "liveFrontEnd");
        return this;
    }

    public Configuration presetTelephoneInput() {
        setLocalProperty("melFilterBank->numberFilters", "31");
        setLocalProperty("melFilterBank->minimumFrequency", "200");
        setLocalProperty("melFilterBank->maximumFrequency", "3500");
    }

    public Configuration 16Khz() {
        setLocalProperty("melFilterBank->numberFilters", "40");
        setLocalProperty("melFilterBank->minimumFrequency", "1");
        setLocalProperty("melFilterBank->maximumFrequency", "8000");
    }

    /**
     * Sets property within a "component" tag in configuration.
     *
     * Use this method to alter "value" property of a "property" tag inside a
     * "component" tag of the XML configuration.
     *
     * @param  name  property name
     * @param  value property value
     * @return       the same instance of {@link Configuration}
     * @see          Configuration#setGlobalProperty(String, Object)
     */
    public Configuration setLocalProperty(String name, Object value) {
        setProperty(configurationManager, name, value.toString());
        return this;
    }

    /**
     * Sets property of a top-level "property" tag.
     *
     * Use this method to alter "value" property of a "property" tag whose
     * parent is the root tag "config" of the XML configuration.
     *
     * @param  name  property name
     * @param  value property value
     * @return       the same instance of {@link Configuration}
     * @see          ConfigurationManager#setLocalProperty(String, Object) 
     */
    public Configuration setGlobalProperty(String name, Object value) {
        configurationManager.setGlobalProperty(name, value.toString());
        return this;
    }

    /**
     * Returns instance of {@link ConfigurationManager}.
     *
     * @see AbstractSpeechRecognizer
     */
    public ConfigurationManager getConfigurationManager() {
        return configurationManager;
    }
}
