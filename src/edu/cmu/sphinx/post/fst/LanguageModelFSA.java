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
import java.io.*;
import java.net.URL;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.acoustic.UnitManager;
import edu.cmu.sphinx.linguist.dictionary.FastDictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.SimpleNGramModel;
import edu.cmu.sphinx.util.LogMath;

/**
 * Language Model FSA - each state contains a ngram and each transition a probability 
 * for the next ngram
 * @author Alexandru Tomescu
 *
 */
public class LanguageModelFSA implements FiniteStateAutomata {
	private LinkedList<State> states;
	private LinkedList<Trans> transitions;
	private HashSet<Word> symbolSet;
	private FastDictionary dict;
	private SimpleNGramModel model;
	LinkedList<WordSequence> NGrams;
	State initialState, finalState;
	
	HashMap<WordSequence, State> state_map;
	Word sbW, seW, epsW, startW;
	WordSequence sbWS, seWS, epsWS, startWS;

	
	/**
	 * 
	 * @param lm_location - location of the language model
	 * @param dict_location - location of the dictionary
	 * @param filler_location -location of the filler dictionary
	 * @throws IOException
	 */
	public LanguageModelFSA(URL lm_location, URL dict_location, URL filler_location) throws IOException {
		// allocate class variables
		allocate(lm_location, dict_location, filler_location);	
		// create words and word sequences that are needed
		createWWS();
		
		// the <start> and <eps> states are added first
		initialState = new State(startWS);
		states.add(initialState);
		state_map.put(startWS, initialState);
		symbolSet.add(startW);
		
		
		State backoff_state = new State(epsWS);
		states.add(backoff_state);		
		state_map.put(epsWS, backoff_state);
		
		Iterator<WordSequence> it = NGrams.iterator();
		
		// Iterate through the ngrams and make states and transitions according to orders
		while (it.hasNext()) {
			WordSequence current = it.next();
			State newState = new State(current);
			state_map.put(current, newState);
			states.add(newState);
			// 1grams
			if (current.size() == 1) {
				Word current_word = current.getWord(0);
				if (current_word.equals(sbW)) {
					this.addTransition(initialState, newState, sbW, 0f, true);
					this.addTransition(newState, state_map.get(epsWS), epsW, model.getBackoff(sbWS), false);
				}
				else if (current_word.equals(seW)) {
					finalState = newState;
					this.addTransition(state_map.get(epsWS), newState, seW, model.getProbability(epsWS), false);
				}
				else {
					float weight = 0.0f;
					if (model.getBackoff(current) != 0) {
						weight = model.getBackoff(current);
					}
					this.addTransition(newState, state_map.get(epsWS), epsW, weight, false);
					this.addTransition(state_map.get(epsWS), newState, current_word, model.getProbability(current), false);
				}
			}
			// middle ngrams
			else if (current.size() < model.getMaxDepth()) {
				if (current.getWord(current.size()-1).equals(seW)) {
					this.addTransition(state_map.get(current.getSubSequence(0, current.size()-1)), state_map.get(seWS), 
							seW, model.getProbability(current), false);					
				}
				else {
					float weight = 0.0f;
					if (model.getBackoff(current) != 0) {
						weight = model.getBackoff(current);
					}
					this.addTransition(newState, state_map.get(current.getSubSequence(1, current.size())), 
							epsW, weight, false);
					this.addTransition(state_map.get(current.getSubSequence(0, current.size()-1)), newState, 
							current.getWord(current.size()-1), model.getProbability(current), false);
				}
			}
			// ngrams
			else if (current.size() == model.getMaxDepth()) {
				if (current.getWord(current.size()-1).equals(seW)) {
					Word[] temp = {current.getWord(current.size()-1)};
					WordSequence temp_seq = new WordSequence(temp);
					this.addTransition(state_map.get(current.getSubSequence(0, current.size()-1)), state_map.get(temp_seq), 
							current.getWord(current.size()-1), model.getProbability(current), false);
				}
				else {
					this.addTransition(state_map.get(current.getSubSequence(0, current.size()-1)), 
							state_map.get(current.getSubSequence(1, current.size())), current.getWord(current.size()-1), 
							model.getProbability(current), false);
				}
			}
		}
		
	}
	
	/**
	 * Add transition between two states containing a word and the probability
	 * @param start - the state from which the transition is made
	 * @param finish - the state to which the transition is made
	 * @param word - the word which is represented by the transition
	 * @param probability - the probability of the obtained ngram
	 * @param first - decide if the transition should be put first in the list of transitions
	 */
	private void addTransition(State start, State finish, Word word, float probability, boolean first) {
		Trans t = new Trans(start, finish, word, probability);
		start.addTransition(t);
		if (!first)
			transitions.add(t);
		else transitions.addFirst(t);
		symbolSet.add(word);
	}
	
	/**
	 * Allocate the needed resources
	 * @param lm_location
	 * @param dict_location
	 * @param filler_location
	 * @throws IOException
	 */
	private void allocate(URL lm_location, URL dict_location, URL filler_location) throws IOException {
		// set up dictionary and language model
		readDictionary(dict_location, filler_location);
		readLanguageModel(lm_location);
		NGrams = model.getNGrams();
		states = new LinkedList<State>();
		transitions = new LinkedList<Trans>();
		symbolSet = new HashSet<Word>();
		state_map = new HashMap<WordSequence, State>();
	}
	
	/**
	 * Create words and word sequences needed
	 */
	private void createWWS() {
		sbW = new Word("<s>", null, false);
		seW = new Word("</s>", null, false);
		epsW = new Word("<eps>", null, false);
		startW = new Word("<start>", null, false);
		
		Word[][] w = {{sbW}, {seW}, {epsW}, {startW}};
		sbWS = new WordSequence(w[0]);
		seWS = new WordSequence(w[1]); 	
		epsWS = new WordSequence(w[2]);
		startWS = new WordSequence(w[3]);
	}
	
	private void readDictionary(URL dict_location, URL filler_location) throws IOException {
		dict = new FastDictionary(dict_location, 
				filler_location, null, false, "<sil>", true, false, new UnitManager());
		
		dict.allocate();
	}
	
	private void readLanguageModel(URL lm_location) throws IOException {
		LogMath logm = new LogMath((float)10, false);
		model = new SimpleNGramModel(lm_location, dict, (float)0.7, logm, 3);
		
		model.allocate();

	}

	/**
	 * Write the transitions to file as an FST: State1 State2 Input_Label Output_Label Probability 
	 */
	public void writeToFile(String path) {
		FileWriter fsaFile = null;
		
		try {
			fsaFile = new FileWriter(path);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedWriter out = new BufferedWriter(fsaFile);
		
		
		for (Trans t : transitions) {
			try {
				out.write(t.getStart().getWords().toString() + " " +
						t.getFinish().getWords().toString() + " " +
						t.getWord().toString() + " " +
						t.getWord().toString() + " " +
						-t.getProbability() + '\n');
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		try {
			out.write("[</s>]");
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
		
		try {
			writeSymbols("<eps>", isyms, symbolId++);
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		
		for (Word s : symbolSet) {
			if (s.toString() != "<eps>") {
				try {
					writeSymbols(s.toString(), isyms, symbolId++);
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
		for (State state : states) {
			try {
				writeSymbols(state.getWords().toString(), ssyms, stateId++);
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
