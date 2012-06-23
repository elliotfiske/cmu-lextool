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

import java.util.Iterator;
import java.util.LinkedList;
import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.dictionary.Word;
/**
 * Create a hyperstring containing words and puctuation
 * @author Alexandru Tomescu
 *
 */
public class FSA {
	private String sentence;
	private LinkedList<State> states;
	private LinkedList<Word> punctuation;
	private int current;
	private boolean process_punctuation;
	/**
	 * 
	 * @param sentence
	 * @param process_punctuation - if true, punctuation will be processed
	 */
	public FSA(String sentence, boolean process_punctuation) {
		this.sentence = sentence;
		this.current = 0;
		this.process_punctuation = process_punctuation;
		this.punctuation = new LinkedList<Word>();
		this.punctuation.add(new Word("<COMMA>", null, false));
		this.punctuation.add(new Word("<PERIOD>", null, false));
		this.punctuation.add(new Word("<NONE>", null, false));
		states = new LinkedList<State>();
		states.add(new State(0, State.Type.s));
				
		String[] words = parseSentence();
		for (String word : words) {
			LinkedList<Word> word_form = new LinkedList<Word>();
			word_form.add(new Word(word.toLowerCase(), null, false));
			word_form.add(new Word(WordUtils.capitalize(word), null, false));
			word_form.add(new Word(word.toUpperCase(), null, false));
			this.addTransitions(word_form);
			if (process_punctuation) {
				this.addTransitions(punctuation);
			}
		}
	}
	
	
	String[] parseSentence() {
		String[] words = this.sentence.split(" "); 
		return words;
	}

	
	private void addTransitions(LinkedList<Word> word) { 
		
		State current_state = states.getLast();
		State st = new State(current, (this.process_punctuation) ? 
				((this.states.getLast().getType() == State.Type.s) ? 
						State.Type.t : State.Type.s) : State.Type.s);
		
		Iterator<Word> it = word.iterator();
		while(it.hasNext()) {
			Trans t = new Trans(current_state, st, it.next());
			current_state.addTransition(t);
		}
		st.setPrev(current_state);
		current_state.setNext(st);
		states.add(st);	
		if (process_punctuation && st.getType() == State.Type.t)
			current++;
		
		if (!process_punctuation)
			current++;
	}
	
	public String toString() {
		State current_state = states.getFirst();
		String s = "";

		while(current_state.getNext() != null) {
			s += current_state.toString() + "--(";
			LinkedList<Trans> transitions = current_state.getTransitions();
			Iterator<Trans> it = transitions.iterator();
			while (it.hasNext()) {
				s += it.next().getWord() + " ";
			}
			s += ")-->";
			current_state = current_state.getNext();
		}
		
		return s;
	}
	
	
	
	
}
