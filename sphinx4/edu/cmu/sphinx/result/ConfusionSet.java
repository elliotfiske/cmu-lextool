/*
 * Copyright 1999-2004 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.result;

import java.util.HashSet;
import java.util.Set;
import java.util.TreeMap;

/**
 * A confusion set is a set of words with their associated posteriors.
 * In Java terms it's a SortedMap from posteriors to sets of WordResults, but
 * the class is called a set because that's what this beast is known as
 * in the literature.
 * 
 * @author pgorniak
 */ 
public class ConfusionSet extends TreeMap {
    
    /**
     * Add a word hypothesis to this confusion set.
     * 
     * @param word the hypothesis to add
     */
    public void addWordHypothesis(WordResult word) {
        Set wordSet = getWordSet(word.getConfidence());
        if (wordSet == null) {
            wordSet = new HashSet();
            put(new Double(word.getConfidence()),wordSet);
        }
        wordSet.add(word);
    }
    
    /**
     * Get the word set with this confidence.
     * 
     * @param posterior the confidence (posterior)
     * @return a set of hypotheses with this confidence, null if no such hypotheses
     * 
     */
    public Set getWordSet(double posterior) {
        return (Set)get(new Double(posterior)); 
    }
    
    /**
     * Return the set of best hypothesis. This will usually contain one
     * hypothesis, but might contain more case there are some that have
     * exactly equal confidences.
     * 
     * @return a set of best hypotheses
     * 
     */
    public Set getBestHypothesisSet() {
        return (Set)get(lastKey()); 
    }
    
    /**
     * Return the single best hypothesis. Breaks ties arbitrarily.
     * 
     * @return the best hypothesis stored in this confusion set (by confidence)
     */
    public WordResult getBestHypothesis() {
        Set s = getBestHypothesisSet();
        return (WordResult)s.iterator().next();
    }
    
    /**
     * Get the highest posterior (confidence) stored in this set.
     * @return the highes posterior
     */
    public double getBestPosterior() {
        return ((Double)lastKey()).doubleValue();
    }
}
