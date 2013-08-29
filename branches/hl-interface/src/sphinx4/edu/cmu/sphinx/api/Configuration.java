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


/**
 * Represents common configuration options.
 *
 * This configuration is used by high-level recognition classes.
 *
 * @see SpeechAligner
 * @see LiveSpeechRecognizer
 * @see BatchSpeechRecognizer
 */
public class Configuration {

    private String logLevel = "WARNING";
    private int absoluteBeamWidth = -1;
    private float relativeBeamWidth = 1e-45f;
    private float wordInsertionProbability = .1f;
    private float silenceInsertionProbability = .9f;
    private float languageWeight = 8.f;

    private String acousticModelPath;
    private String dictionaryPath;
    private String languageModelPath;
    private String grammarPath;
    private String grammarName;

    private boolean useGrammar = false;

    /**
     * Returns log level.
     */
    public String getLogLevel() {
       return logLevel;
    }

    /**
     * Sets log level.
     */
    public void setLogLevel(String logLevel) {
        this.logLevel = logLevel;
    }

    /**
     * Returns absolute beam width.
     */
    public int getAbsoluteBeamWidth() {
       return absoluteBeamWidth;
    }

    /**
     * Sets absolute beam width.
     */
    public void setAbsoluteBeamWidth(int absoluteBeamWidth) {
        this.absoluteBeamWidth = absoluteBeamWidth;
    }

    /**
     * Returns relative beam width.
     */
    public float getRelativeBeamWidth() {
       return relativeBeamWidth;
    }

    /**
     * Sets relative beam width.
     */
    public void setRelativeBeamWidth(float relativeBeamWidth) {
        this.relativeBeamWidth = relativeBeamWidth;
    }

    /**
     * Retruns word insertion probability.
     */
    public float getWordInsertionProbability() {
       return wordInsertionProbability;
    }

    /**
     * Sets word insertion probability.
     */
    public void setWordInsertionProbability(float wordInsertionProbability) {
        this.wordInsertionProbability = wordInsertionProbability;
    }

    /**
     * Returns silence insertion probability.
     */
    public float getSilenceInsertionProbability() {
       return silenceInsertionProbability;
    }

    /**
     * Sets silence insertion probability.
     */
    public void setSilenceInsertionProbability(
            float silenceInsertionProbability)
    {
        this.silenceInsertionProbability = silenceInsertionProbability;
    }

    /**
     * Returns language weight.
     */
    public float getLanguageWeight() {
       return languageWeight;
    }

    /**
     * Sets language weight.
     */
    public void setLanguageWeight(float languageWeight) {
        this.languageWeight = languageWeight;
    }

    /**
     * Returns path to acoustic model.
     */
    public String getAcousticModelPath() {
        return acousticModelPath;
    }

    /**
     * Sets path to acoustic model.
     */
    public void setAcousticModelPath(String acousticModelPath) {
        this.acousticModelPath = acousticModelPath;
    }

    /**
     * Returns path to dictionary.
     */
    public String getDictionaryPath() {
        return dictionaryPath;
    }

    /**
     * Sets path to dictionary.
     */
    public void setDictionaryPath(String dictionaryPath) {
        this.dictionaryPath = dictionaryPath;
    }

    /**
     * Returns path to language model.
     */
    public String getLanguageModelPath() {
        return languageModelPath;
    }

    /**
     * Sets paths to language model resource.
     */
    public void setLanguageModelPath(String languageModelPath) {
        this.languageModelPath = languageModelPath;
    }

    /**
     * Returns path to grammar resources.
     */
    public String getGrammarPath() {
        return grammarPath;
    }

    /**
     * Sets path to grammar resources.
     */
    public void setGrammarPath(String grammarPath) {
        this.grammarPath = grammarPath;
    }

    /**
     * Returns grammar name.
     */
    public String getGrammarName() {
        return grammarName;
    }

    /**
     * Sets grammar name if fixed grammar is used.
     */
    public void setGrammarName(String grammarName) {
        this.grammarName = grammarName;
    }

    /**
     * Returns whether fixed grammar should be used instead of language model.
     */
    public boolean getUseGrammar() {
        return useGrammar;
    }

    /**
     * Sets whether fixed grammar should be used instead of language model.
     */
    public void setUseGrammar(boolean useGrammar) {
        this.useGrammar = useGrammar;
    }
}
