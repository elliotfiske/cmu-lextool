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

package edu.cmu.sphinx.post.dynamic;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Word;


public class Sequence implements Comparable<Sequence>{
	private WordSequence words;
	private float probability;
	private int size;
	private Sequence previous;
	
	public Sequence(WordSequence words, float probability, int sequenceNumber, Sequence previous) {
		this.words = words;
		this.size = sequenceNumber;
		if (previous != null)  {
			this.probability = previous.getProbability() + probability;
		}
		else {
			this.probability = 0;
		}
	}
	
	public int getSize() {
		return this.size;
	}
	
	public int getSequenceNumber() {
		return this.size;
	}
	
	public float getProbability() {
		return this.probability;
	}
	
	public Word[] getWords() {
		return this.words.getWords();
	}
	
	public WordSequence getWordSequence() {
		return this.words;
	}
	
	public String toString() {
		return words.toString();
	}
	
	public int compareTo(Sequence s) {
		if (this.probability > s.getProbability()) return -1;
		else if (this.probability == s.getProbability()) return 0;
		return 1;
	}
	
	public Sequence getPrev() {
		return this.previous;
	}
}
