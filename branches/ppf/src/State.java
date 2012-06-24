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

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Iterator;
import java.util.LinkedList;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Word;


/**
 * State objects are the nodes in the FSA
 * @author Alexandru Tomescu
 *
 */
public class State {
	public enum Type {
		s, t
	}
	
	private boolean languageModel;
	private int seq;
	private Type type;
	private State nextState, previousState;
	private LinkedList<Trans> transitions;
	private WordSequence words;
	
	public State() {
		transitions = new LinkedList<Trans>();
		nextState = null;
		previousState = null;
		languageModel = true;
	}
	
	public State(WordSequence words) {
		this.words = words;
		transitions = new LinkedList<Trans>();
		nextState = null;
		previousState = null;
		languageModel = true;
	}
	
	public State(int seq, Type type) {
		this.seq = seq;
		this.type = type;
		transitions = new LinkedList<Trans>();
		nextState = null;
		previousState = null;
		languageModel = false;
	}
	
	public void addTransition(Trans t) {
		transitions.add(t);
		if (languageModel == true) {
			try {
				Main.out.write(
						t.getStart().words.toString() + '\t' + 
						t.getFinish().words.toString() + '\t' + 
						t.getWord().toString() + '\t' + 
						t.getProbability() + '\n');
			} catch (IOException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}
		
		
	}
	
	public LinkedList<Trans> getTransitions() {
		return this.transitions;
	}
	
	public void setNext(State s) {
		this.nextState = s;
	}
	
	public State getNext() {
		return this.nextState;
	}
	
	public void setPrev(State s) {
		this.previousState = s;
	}
	
	public State getPrev() {
		return this.previousState;
	}
	
	public void setType(Type t) {
		this.type = t;
	}
	
	public Type getType() {
		return this.type;
	}
	
	public void setSeq(int s) {
		this.seq = s;
	}
	
	public int getSeq() {
		return this.seq;
	}
	
	public String toString() {
		return this.type + "" + this.seq;
	}
	
	public State containsTransition(Word w) {
		Iterator<Trans> it = transitions.iterator();
		while (it.hasNext()) {
			Trans t = it.next();
			if (t.getWord().equals(w)) 
				return t.getFinish();
		}
		return null;
	}
	
	public WordSequence getWords() {
		return this.words;
	}
}
