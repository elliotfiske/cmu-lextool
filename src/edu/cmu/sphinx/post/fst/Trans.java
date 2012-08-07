package edu.cmu.sphinx.post.fst;
import edu.cmu.sphinx.linguist.dictionary.Word;


/*                                                                              
 * 
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

/**
 * Trans(itions) are the edges in the hyperstring
 * @author Alexandru Tomescu
 *
 */
public class Trans {
	private State start;
	private State finish;
	
	private Word word;
	private float probability;
	
	public Trans (State start, State finish, Word word) {
		this.start = start;
		this.finish = finish;
		this.word = word;
	}
	
	public Trans (State start, State finish, Word word, float prob) {
		this.start = start;
		this.finish = finish;
		this.word = word;
		this.probability = prob;
	}
	
	void setStart(State start) {
		this.start = start;
	}
	
	State getStart() {
		return this.start;
	}
	
	void setFinish(State finish) {
		this.finish = finish;
	}
	
	State getFinish() {
		return this.finish;
	}
	
	void setWord(Word word) {
		this.word = word;
	}
	
	Word getWord() {
		return this.word;
	}
	
	void setProbability(float prob) {
		this.probability = prob;
	}
	
	float getProbability() {
		return this.probability;
	}
}
