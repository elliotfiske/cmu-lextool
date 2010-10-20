/*
 * Copyright 1999-2002 Carnegie Mellon University.
 * Portions Copyright 2002 Sun Microsystems, Inc.
 * Portions Copyright 2002 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.result;

import edu.cmu.sphinx.frontend.Feature;
import edu.cmu.sphinx.frontend.Utterance;
import edu.cmu.sphinx.decoder.linguist.WordSearchState;
import edu.cmu.sphinx.decoder.search.ActiveList;
import edu.cmu.sphinx.decoder.search.Token;
import edu.cmu.sphinx.decoder.search.AlternateHypothesisManager;
import edu.cmu.sphinx.knowledge.dictionary.Word;

import java.util.List;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.ArrayList;

/**
 * Provides recognition results. Results can be partial or final. A
 * result should not be modified before it is a final result. Note
 * that a result may not contain all possible information.
 *
 *  The following methods are not yet defined but should be:
 * <pre>
 public Result getDAG(int compressionLevel);
 </pre>
 *
 */
public class Result {

    private ActiveList activeList;
    private List resultList;
    private AlternateHypothesisManager alternateHypothesisManager;
    private boolean isFinal = false;
    private int currentFrameNumber;

    /**
     * Creates a result
     *
     * @param activeList the active list associated with this result
     * @param resultList the result list associated with this result
     * @param frameNumber the frame number for this result.
     * @param isFinal if true, the result is a final result
     */
    public Result(AlternateHypothesisManager alternateHypothesisManager,
                  ActiveList activeList, List resultList, int frameNumber,
                  boolean isFinal) {
        this(activeList, resultList, frameNumber, isFinal);
        this.alternateHypothesisManager = alternateHypothesisManager;
    }

    /**
     * Creates a result
     *
     * @param activeList the active list associated with this result
     * @param resultList the result list associated with this result
     * @param frameNumber the frame number for this result.
     * @param isFinal if true, the result is a final result
     */
    public Result(ActiveList activeList, List resultList, int frameNumber,
                  boolean isFinal) {
        this.activeList = activeList;
        this.resultList = resultList;
        this.currentFrameNumber = frameNumber;
        this.isFinal = isFinal;
    }

    /**
     * Determines if the result is a final result.  A final result is
     * guaranteed to no longer be modified by the SearchManager that
     * generated it. Non-final results can be modifed by a
     * <code>SearchManager.recognize</code> calls.
     *
     * @return true if the result is a final result
     */
    public boolean isFinal() {
        return isFinal;
    }

    /**
     * Returns a list of active tokens for this result. The list
     * contains zero or active <code>Token</code> objects that
     * represents the leaf nodes of all active branches in the result
     * (sometimes referred to as the 'lattice').
     *
     * The lattice is live and may be modified by a
     * SearchManager during a recognition.  Once the Result is final,
     * the lattice is fixed and will no longer be modified by the
     * SearchManager. Applications can modify the lattice (to prepare
     * for a re-recognition, for example) only after
     * <code>isFinal</code> returns <code>true</code>
     *
     * @return a list containing the active tokens for this result
     *
     * @see Token
     */
    public ActiveList getActiveTokens() {
        return activeList;
    }

    /**
     * Returns a list of result tokens for this result. The list
     * contains zero or more result <code>Token</code> objects that
     * represents the leaf nodes of all final branches in the result
     * (sometimes referred to as the 'lattice').
     *
     * The lattice is live and may be modified by a
     * SearchManager during a recognition.  Once the Result is final,
     * the lattice is fixed and will no longer be modified by the
     * SearchManager. Applications can modify the lattice (to prepare
     * for a re-recognition, for example) only after
     * <code>isFinal</code> returns <code>true</code>
     *
     * @return a list containing the final result tokens for this result
     *
     * @see Token
     */
    public List getResultTokens() {
        return resultList;
    }

    /**
     * Returns the AlternateHypothesisManager
     * Used to construct a Lattice
     *
     * @return the AlternateHypothesisManager
     */
    public AlternateHypothesisManager getAlternateHypothesisManager() {
        return alternateHypothesisManager;
    }


    /**
     * Returns the current frame number
     *
     * @return the frame number
     */
    public int getFrameNumber() {
        return currentFrameNumber;
    }

    /**
     * Returns the best scoring token in the result
     *
     * @return the best scoring token or null
     */
    public Token getBestToken() {
        Token bestToken = null;
        for (Iterator i = resultList.iterator(); i.hasNext();) {
            Token token = (Token) i.next();
            if (bestToken == null || token.getScore() > bestToken.getScore()) {
                bestToken = token;
            }
        }

        if (bestToken == null) {
            bestToken = getBestActiveToken();
        }

        return bestToken;
    }

    /**
     * Returns the best scoring token in the active set
     *
     * @return the best scoring token or null
     */
    public Token getBestActiveToken() {
        Token bestToken = null;
        for (Iterator i = activeList.iterator(); i.hasNext();) {
            Token token = (Token) i.next();
            if (bestToken == null || token.getScore() > bestToken.getScore()) {
                bestToken = token;
            }
        }
        return bestToken;
    }

    /**
     * Searches through the n-best list to find the
     * the branch that matches the given string
     *
     * @param text the string to search for
     * @return the token at the head of the branch or null
     */
    public Token findToken(String text) {
        text = text.trim();
        for (Iterator i = resultList.iterator(); i.hasNext();) {
            Token token = (Token) i.next();
            if (text.equals(token.getWordPathNoFiller())) {
                return token;
            }
        }
        return null;
    }

    /**
     * Searches through the n-best list to find the
     * the branch that matches the beginning of the given  string
     *
     * @param text the string to search for
     * @return the list token at the head of the branch
     */
    public List findPartialMatchingTokens(String text) {
        List list = new ArrayList();
        text = text.trim();
        for (Iterator i = activeList.iterator(); i.hasNext();) {
            Token token = (Token) i.next();
            if (text.startsWith(token.getWordPathNoFiller())) {
                list.add(token);
            }
        }
        return list;
    }

    /**
     * Returns the best scoring token that matches the beginning of
     * the given text.
     *
     * @param text the text to match
     */
    public Token getBestActiveParitalMatchingToken(String text) {
        List matchingList = findPartialMatchingTokens(text);
        Token bestToken = null;
        for (Iterator i = matchingList.iterator(); i.hasNext();) {
            Token token = (Token) i.next();
            if (bestToken == null || token.getScore() > bestToken.getScore()) {
                bestToken = token;
            }
        }
        return bestToken;
    }


    /**
     * Returns detailed frame statistics for this result
     *
     * @return frame statistics for this result as an array, with one
     * element per frame or <code>null</code> if no frame statistics
     * are available.
     */
    public FrameStatistics[] getFrameStatistics() {
        return null;	// [[[ TBD:  write me ]]]
    }


    /**
     * Returns the set of best paths for this result.
     *
     * @param numBestPaths the maximum number of paths returned
     *
     * @return an array containing at most <code>numBestPaths</code>
     * paths.
     */
    public Path[] getBestPaths(int numBestPaths) {
        return null;	// [[[ TBD:  write me ]]]
    }

    /**
     * Gets the starting frame number for the result
     *
     * @return the starting frame number for the result
     */
    public int getStartFrame() {
        return 0;
    }

    /**
     * Gets the ending frame number for the result
     *
     * @return the ending frame number for the result
     */
    public int getEndFrame() {
        return 0;	// [[[ TBD: write me ]]]
    }

    /**
     * Gets the feature frames associated with this result
     *
     * @return the set of feature frames associated with this result,
     *    or null if
     * the frames are not available.
     */
    public Feature[] getFeatureFrames() {
        Feature[] features = null;

        // find the best token, and then trace back for all the features
        Token token = getBestToken();
        if (token != null) {
            List featureList = new LinkedList();
            do {
                Feature feature = token.getFeature();
                featureList.add(0, feature);
                token = token.getPredecessor();
            } while (token != null);

            features = new Feature[featureList.size()];
            featureList.toArray(features);
        }
        return features;
    }


    /**
     * Returns the Utterance (that is, the audio) associated with this Result.
     * The Utterance will be available ONLY if the SphinxProperties
     * <pre>edu.cmu.sphinx.frontend.keepAudioReference</pre> is true.
     *
     * @return the Utterance associated with this result, or null if the
     * Utterance is not available.
     */
    public Utterance getUtterance() {
        Utterance utterance = null;
        Token token = getBestToken();

        if (token != null) {
            // first trace back to a token that has a Feature
            while (token != null && token.getFeature() == null) {
                token = token.getPredecessor();
            }

            // from the Feature, obtain a reference to the Utterance
            if (token != null) {
                Feature feature = token.getFeature();
                if (feature != null) {
                    utterance = feature.getUtterance();
                }
            }
        }
        return utterance;
    }

    /**
     * Returns the string of the best result, removing any filler words.
     *
     * @return the string of the best result, removing any filler
     * words
     */
    public String getBestResultNoFiller() {
        Token token = getBestToken();
        if (token == null) {
            return "";
        } else {
            return token.getWordPathNoFiller();
        }
    }

    /**
     * Returns the string of words (with timestamp) for this token.
     *
     * @param wantFiller true if we want filler words included, false otherwise
     * @param wordTokenFirst true if the word tokens come before other types
     *     of tokens
     *
     * @return the string of words
     */
    public String getTimedBestResult(boolean wantFiller,
                                     boolean wordTokenFirst,
                                     int sampleRate) {
        Token token = getBestToken();
        if (token == null) {
            return "";
        } else {
            if (wordTokenFirst) {
                return getTimedWordPath(token, wantFiller, sampleRate);
            } else {
                return getTimedWordTokenLastPath(token, wantFiller, 
                                                 sampleRate);
            }
        }
    }

    /**
     * Returns the string of words (with timestamp) for this token.
     * This method assumes that the word tokens come before other types
     * of token.
     *
     * @param wantFiller true if we want filler words, false otherwise
     *
     * @return the string of words
     */
    private String getTimedWordPath(Token token, boolean wantFiller,
                                    int sampleRate) {
        StringBuffer sb = new StringBuffer();

        // get to the first emitting token
        while (token != null & !token.isEmitting()) {
            token = token.getPredecessor();
        }

        if (token != null) {
            Feature lastWordFirstFeature = token.getFeature();
            Feature lastFeature = lastWordFirstFeature;
            token = token.getPredecessor();

            while (token != null) {
                if (token.isWord()) {
                    Word word = token.getWord();
                    if (wantFiller || !word.isFiller()) {
                        addWord(sb, word, lastFeature, lastWordFirstFeature,
                                sampleRate);
                    }
                    lastWordFirstFeature = lastFeature;
                }
                Feature feature = token.getFeature();
                if (feature != null) {
                    lastFeature = feature;
                }
                token = token.getPredecessor();
            }
        }
        return sb.toString();
    }


    /**
     * Returns the string of words for this token, each with the starting
     * sample number as the timestamp. This method assumes that the word
     * tokens come after the unit and hmm tokens.
     *
     * @return the string of words, each with the starting sample number
     */
    private String getTimedWordTokenLastPath(Token token, boolean wantFiller,
                                             int sampleRate) {
        StringBuffer sb = new StringBuffer();
        Word word = null;
        Feature lastFeature = null;
        Feature lastWordFirstFeature = null;

        while (token != null) {
            if (token.isWord()) {
                if (word != null) {
                    if (wantFiller || !word.isFiller()) {
                        addWord(sb, word, lastFeature, lastWordFirstFeature,
                                sampleRate);
                    }
                    word = token.getWord();
                    lastWordFirstFeature = lastFeature;
                }
            }
            Feature feature = token.getFeature();
            if (feature != null) {
                lastFeature = feature;
                if (lastWordFirstFeature == null) {
                    lastWordFirstFeature = lastFeature;
                }
            }
            token = token.getPredecessor();
        }

        return sb.toString();
    }

    /**
     * Adds the given word into the given string buffer with the start and
     * end times from the given features.
     *
     * @param sb the StringBuffer into which the word is added
     * @param word the word to add
     * @param startFeature the starting feature
     * @param endFeature tne ending feature
     * @param sampleRate the sample rate of the data
     */
    private void addWord(StringBuffer sb, Word word,
                         Feature startFeature, Feature endFeature,
                         int sampleRate) {
        float startTime = 
            ((float) startFeature.getFirstSampleNumber()/sampleRate);
        float endTime =
            ((float) endFeature.getFirstSampleNumber()/sampleRate);
        if (sb.length() > 0) {
            sb.insert(0, " ");
        }
        sb.insert(0, (word.getSpelling() + "(" + startTime + "," + 
                      endTime + ")"));
    }


    /**
     * Returns a string representation of this object
     */
    public String toString() {
        Token token = getBestToken();
        if (token == null) {
            return "";
        } else {
            return token.getWordPath();
        }
    }


    /**
     * Sets the results as a final result
     *
     * @param finalResult if true, the result should be made final
     */
    void setFinal(boolean finalResult) {
        this.isFinal = finalResult;
    }


    /**
     * Determines if the Result is valid. This is used for testing and
     * debugging
     *
     * @return true if the result is properly formed.
     *
     */
    public boolean validate() {
        boolean valid = true;
        for (Iterator i = activeList.iterator(); i.hasNext();) {
            Token token = (Token) i.next();
            if (!token.validate()) {
                valid = false;
                token.dumpTokenPath();
            }
        }
        return valid;
    }
}
