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
	
	public static FileWriter FSA_output;
	public static BufferedWriter out;
	public static FileWriter isyms;
	public static BufferedWriter isyms_out;
	public static FileWriter ssyms;
	public static BufferedWriter ssyms_out;
	int i, j;
	/**
	 * 
	 * @param sentence
	 * @param process_punctuation - if true, punctuation will be processed
	 * @throws IOException 
	 */
	public FSA(String sentence, boolean process_punctuation) throws IOException {
		
		allocate(sentence, process_punctuation);
		
		// write punctuation marks as symbols
		isyms_out.write(new Word("<COMMA>", null, false) + " " + j++ + '\n');
		isyms_out.write(new Word("<PERIOD>", null, false) + " " + j++ +'\n');
		isyms_out.write(new Word("<NONE>", null, false) + " " + j++ + '\n');
		
		states.add(new State(0, State.Type.s));
		ssyms_out.write("s0 0 \n");
				
		String[] words = parseSentence();
		for (String word : words) {
			LinkedList<Word> word_form = new LinkedList<Word>();
			word_form.add(new Word(word.toLowerCase(), null, false));
			isyms_out.write(word.toLowerCase() + " " + j++ + '\n');
			word_form.add(new Word(WordUtils.capitalize(word), null, false));
			isyms_out.write(WordUtils.capitalize(word) + " " + j++ + '\n');
			word_form.add(new Word(word.toUpperCase(), null, false));
			isyms_out.write(word.toUpperCase() + " " + j++ + '\n');
			
			this.addTransitions(word_form);
			if (process_punctuation) {
				this.addTransitions(punctuation);
			}
		}
		out.close();
		ssyms_out.close();
		isyms_out.close();
	}
	
	private void allocate(String sentence, boolean process_punctuation) throws IOException {
		FSA_output = new FileWriter("hyperstring_fsa");
	    out = new BufferedWriter(FSA_output);
	    isyms = new FileWriter("hyperstring_fsa_isyms");
		isyms_out = new BufferedWriter(isyms);
		ssyms = new FileWriter("hyperstring_fsa_ssyms");
		ssyms_out = new BufferedWriter(ssyms);
		i = 1; j = 0;
		states = new LinkedList<State>();
		this.sentence = sentence;
		this.current = 0;
		this.process_punctuation = process_punctuation;
		this.punctuation = new LinkedList<Word>();
		this.punctuation.add(new Word("<COMMA>", null, false));
		this.punctuation.add(new Word("<PERIOD>", null, false));
		this.punctuation.add(new Word("<NONE>", null, false));
	}
	
	String[] parseSentence() {
		String[] words = this.sentence.split(" "); 
		return words;
	}

	
	private void addTransitions(LinkedList<Word> word) throws IOException { 
		
		State current_state = states.getLast();
		State st = new State(current, (this.process_punctuation) ? 
				((this.states.getLast().getType() == State.Type.s) ? 
						State.Type.t : State.Type.s) : State.Type.s);
		ssyms_out.write(st.getType().toString() + st.getSeq() + " " + i++ + '\n');
		
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
