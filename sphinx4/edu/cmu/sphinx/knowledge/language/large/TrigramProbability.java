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

package edu.cmu.sphinx.knowledge.language.large;


/**
 * Represents a word ID (third word of a trigram), and a trigram 
 * probability ID.
 */
public class TrigramProbability {

    private int wordID;
    private int probabilityID;


    /**
     * Constructs a TrigramProbability
     *
     * @param wordID the ID of the second word in a bigram
     * @param probabilityID the index into the probability array
     */
    public TrigramProbability(int wordID, int probabilityID) {
	this.wordID = wordID;
        this.probabilityID = probabilityID;
    }

    
    /**
     * Returns the second word ID of this bigram
     *
     * @return the second word ID
     */ 
    public int getWordID() {
        return wordID;
    }


    /**
     * Returns the bigram probability ID.
     *
     * @return the bigram probability ID
     */
    public int getProbabilityID() {
        return probabilityID;
    }
}
