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
package edu.cmu.sphinx.post.fst;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.dictionary.Word;
/**
 * Create a hyperstring containing words and puctuation
 * @author Alexandru Tomescu
 *
 */
public class FSA implements FiniteStateAutomata {
	private String sentence;
	private LinkedList<State> states;
	private LinkedList<Trans> transitions;
	private HashSet<Word> symbolSet;
	private Word[] punctuation = {new Word("<COMMA>", null, false), 
			new Word("<PERIOD>", null, false),
			new Word("<NONE>", null, false)};
	private int currentStateId = 0;
	private boolean processPunctuation;
	private State.Type lastLabel = State.Type.s;
	State initialState, finalState;
	
	
	/**
	 * 
	 * @param sentence
	 * @param processPunctuation - if true, punctuation will be processed
	 * @throws IOException 
	 */
	public FSA(String sentence, boolean processPunctuation) throws IOException {
		
		states = new LinkedList<State>();
		transitions = new LinkedList<Trans>();
		symbolSet = new HashSet<Word>();
		this.sentence = sentence;
		this.processPunctuation = processPunctuation;
		
		initialState = new State(-1, State.Type.s);
		states.add(initialState);
		Word[] sentenceStart = {new Word("<s>", null, false)};
		this.addTransitions(sentenceStart);
		symbolSet.add(new Word("<s>", null, false));
		
		String[] words = this.sentence.split(" ");
		
		for (String word : words) {
			Word[] wordForms = {new Word(word.toLowerCase(), null, false),
					new Word(WordUtils.capitalize(word), null, false),
					new Word(word.toUpperCase(), null, false)};
			for (Word w : wordForms) {
				symbolSet.add(w);
			}
			
			this.addTransitions(wordForms);
			if (processPunctuation) {
				this.addTransitions(punctuation);
			}
		}
		
		Word[] sentenceEnd = {new Word("</s>", null, false)};
		this.addTransitions(sentenceEnd);
		symbolSet.add(new Word("</s>", null, false));
		transitions.getFirst().setProbability(0.1f);
	}
	
	/**
	 * Add new transitions from the last state to a new one.
	 * @param words - Each word represents a transition
	 * @throws IOException
	 */
	private void addTransitions(Word[] words) throws IOException { 
		
		State currentState = states.getLast();
		
		State.Type label = processPunctuation && currentState.getSeq() != -1 ? 
				State.getOtherLabel(lastLabel) : lastLabel;
		
		State nextState = new State(currentStateId, label);
		
		for (Word word : words) {
			Trans t = new Trans(currentState, nextState, word);
			currentState.addTransition(t);
			transitions.add(t);
		}
		
		nextState.setPrev(currentState);
		currentState.setNext(nextState);
		states.add(nextState);	
		
		lastLabel = nextState.getType();
		
		if (processPunctuation && nextState.getType() == State.Type.t) {
			currentStateId++;
		} else if (!processPunctuation)
			currentStateId++;
		
		finalState = nextState;
	}
	
	public String toString() {
		State currentState = states.getFirst();
		String s = "";

		while(currentState.getNext() != null) {
			s += currentState.toString() + "--(";
			LinkedList<Trans> transitions = currentState.getTransitions();
			Iterator<Trans> it = transitions.iterator();
			while (it.hasNext()) {
				s += it.next().getWord() + " ";
			}
			s += ")-->";
			currentState = currentState.getNext();
		}
		
		return s;
	}

	/**
	 * Write the transitions to file as an FST: State1 State2 Input_Label Output_Label Probability 
	 */
	public void writeToFile(String path) {
		FileWriter fsaFile = null;
		String lastState = null;
		
		try {
			fsaFile = new FileWriter(path);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedWriter out = new BufferedWriter(fsaFile);
		
		for (Trans t : transitions) {
			try {
				out.write(t.getStart().toString() + " " +
						t.getFinish().toString() + " " +
						t.getWord().toString() + " " +
						t.getWord().toString() + " " +
						t.getProbability() + '\n');
			} catch (IOException e) {
				e.printStackTrace();
			}
			lastState = t.getFinish().toString();
		}
		
		try {
			out.write(lastState);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		
		
	}

	/**
	 * Write input/output and state symbols to file: Symbol Id
	 */
	public void writeSymbolsToFile(String inputSymbolsPath,
			String outputSymbolsPath) {
		
		int symbolId = 0;
		int stateId = 0;
		FileWriter inputFile = null;
		FileWriter stateFile = null;
		
		
		try {
			inputFile = new FileWriter(inputSymbolsPath);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			stateFile = new FileWriter(outputSymbolsPath);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedWriter isyms = new BufferedWriter(inputFile);
		BufferedWriter ssyms = new BufferedWriter(stateFile);
		
		symbolSet.add(punctuation[0]);
		symbolSet.add(punctuation[1]);
		symbolSet.add(punctuation[2]);
		
		for (Word s : symbolSet) {
			try {
				writeSymbols(s.toString(), isyms, symbolId++);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		for (State state : states) {
			try {
				writeSymbols(state.toString(), ssyms, stateId++);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		try {
			isyms.close();
			ssyms.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void writeSymbols(String symbolString, BufferedWriter writer, int symbolId) throws IOException {
		Word symbol = new Word(symbolString, null, false);
		
		writer.write(symbol.toString() + " " + symbolId + '\n');
	}
	
	public State getInitialState() {
		return this.initialState;
	}
	
	public State getFinalState() {
		return this.finalState;
	}
	
	public LinkedList<State> getStates() {
		return this.states;
	}
	
	public HashSet<Word> getSymbols() {
		return this.symbolSet;
	}
}
