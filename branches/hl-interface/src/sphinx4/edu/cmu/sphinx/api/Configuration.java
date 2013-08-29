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

    public String getLogLevel() {
       return logLevel;
    }

    public void setLogLevel(String logLevel) {
        this.logLevel = logLevel;
    }

    public int getAbsoluteBeamWidth() {
       return absoluteBeamWidth;
    }

    public void setAbsoluteBeamWidth(int absoluteBeamWidth) {
        this.absoluteBeamWidth = absoluteBeamWidth;
    }

    public float getRelativeBeamWidth() {
       return relativeBeamWidth;
    }

    public void setRelativeBeamWidth(float relativeBeamWidth) {
        this.relativeBeamWidth = relativeBeamWidth;
    }

    public float getWordInsertionProbability() {
       return wordInsertionProbability;
    }

    public void setWordInsertionProbability(float wordInsertionProbability) {
        this.wordInsertionProbability = wordInsertionProbability;
    }

    public float getSilenceInsertionProbability() {
       return silenceInsertionProbability;
    }

    public void setSilenceInsertionProbability(
            float silenceInsertionProbability)
    {
        this.silenceInsertionProbability = silenceInsertionProbability;
    }

    public float getLanguageWeight() {
       return languageWeight;
    }

    public void setLanguageWeight(float languageWeight) {
        this.languageWeight = languageWeight;
    }

    public String getAcousticModelPath() {
        return acousticModelPath;
    }

    public void setAcousticModelPath(String acousticModelPath) {
        this.acousticModelPath = acousticModelPath;
    }

    public String getDictionaryPath() {
        return dictionaryPath;
    }

    public void setDictionaryPath(String dictionaryPath) {
        this.dictionaryPath = dictionaryPath;
    }

    public String getLanguageModelPath() {
        return languageModelPath;
    }

    public void setLanguageModelPath(String languageModelPath) {
        this.languageModelPath = languageModelPath;
    }

    public String getGrammarPath() {
        return grammarPath;
    }

    public void setGrammarPath(String grammarPath) {
        this.grammarPath = grammarPath;
    }

    public String getGrammarName() {
        return grammarName;
    }

    public void setGrammarName(String grammarName) {
        this.grammarName = grammarName;
    }

    public boolean getUseGrammar() {
        return useGrammar;
    }

    public void setUseGrammar(boolean useGrammar) {
        this.useGrammar = useGrammar;
    }
}
