/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.linguist.language.ngram.large4;


/**
 * Represents a word ID (thirs word of a bigram), a bigram probability ID,
 * a backoff probability ID, and the location of the first quadrigram entry.
 */
class TrigramProbability {

    private int which;
    private int wordID;
    private int probabilityID;
    private int backoffID;
    private int firstQuadrigramEntry;


    /**
     * Constructs a QuadrigramProbability
     *
     * @param which which follower of the first word is this trigram
     * @param wordID the ID of the 3 word in a bigram
     * @param probabilityID the index into the probability array
     * @param backoffID the index into the backoff probability array
     * @param firstTrigramEntry the first trigram entry
     */
    public TrigramProbability(int which, int wordID, 
			     int probabilityID, int backoffID,
                             int firstQuadrigramEntry) {
        this.which = which;
	this.wordID = wordID;
        this.probabilityID = probabilityID;
        this.backoffID = backoffID;
        this.firstQuadrigramEntry = firstQuadrigramEntry;
    }

    
    /**
     * Returns which follower of xx word is this trigram
     *
     * @return which follower of the first word is this trigram
     */
    public int getWhichFollower() {
	return which;
    }


    /**
     * Returns the third word ID of this bigram
     *
     * @return the third word ID
     */ 
    public int getWordID() {
        return wordID;
    }


    /**
     * Returns the trigram probability ID.
     *
     * @return the trigram probability ID
     */
    public int getProbabilityID() {
        return probabilityID;
    }


    /**
     * Returns the backoff weight ID.
     *
     * @return the backoff weight ID
     */
    public int getBackoffID() {
        return backoffID;
    }


    /**
     * Returns the index of the first trigram entry.
     *
     * @return the index of the first trigram entry
     */
    public int getFirstQuadrigramEntry() {
        return firstQuadrigramEntry;
    }
}
