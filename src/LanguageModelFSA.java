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

import java.io.*;
import java.net.URL;
import java.util.HashMap;
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
public class LanguageModelFSA {
	private LinkedList<State> states;
	private FastDictionary dict;
	private SimpleNGramModel model;
	LinkedList<WordSequence> NGrams;
	
	HashMap<WordSequence, State> state_map;
	Word sbW, seW, epsW, startW;
	WordSequence sbWS, seWS, epsWS, startWS;
	int i, j;
	
	public static FileWriter FSA_output;
	public static BufferedWriter out;
	public static FileWriter isyms;
	public static BufferedWriter isyms_out;
	public static FileWriter ssyms;
	public static BufferedWriter ssyms_out;
	
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
		State firstState = new State(startWS);
		states.add(firstState);
		state_map.put(startWS, firstState);
		
		
		State backoff_state = new State(epsWS);
		states.add(backoff_state);		
		state_map.put(epsWS, backoff_state);
		
		// the <start> and <eps> states are written in the symbol files
		ssyms_out.write(backoff_state.getWords() + " " + 0 + '\n');
		ssyms_out.write(firstState.getWords() + " " + i++ + '\n');
		isyms_out.write(epsW.toString() + " " + 0 + '\n');
		isyms_out.write(startW.toString() + " " + j++ + '\n');
		
		
		Iterator<WordSequence> it = NGrams.iterator();
		
		while (it.hasNext()) {
			WordSequence current = it.next();
			State new_state = new State(current);
			state_map.put(current, new_state);
			states.add(new_state);
			ssyms_out.write(new_state.getWords() + " " + i++ + '\n');
			if (current.size() == 1) {
				Word current_word = current.getWord(0);
				isyms_out.write(current_word.toString() + " " + j++ + '\n');
				if (current_word.equals(sbW)) {
					firstState.addTransition(new Trans(firstState, new_state, sbW, 0));
					new_state.addTransition(new Trans(new_state, state_map.get(epsWS), epsW, model.getBackoff(sbWS)));
				}
				else if (current_word.equals(seW)) {
					new_state.addTransition(new Trans(state_map.get(epsWS), new_state, seW, model.getProbability(epsWS)));
				}
				else {
					float weight = 0.0f;
					if (model.getBackoff(current) != 0) {
						weight = model.getBackoff(current);
					}
					new_state.addTransition(new Trans(new_state, state_map.get(epsWS), epsW, weight));
					new_state.addTransition(new Trans(state_map.get(epsWS), new_state, current_word, model.getProbability(current)));
				}
			}
			else if (current.size() < model.getMaxDepth()) {
				if (current.getWord(current.size()-1).equals(seW)) {
					new_state.addTransition(new Trans(state_map.get(current.getSubSequence(0, current.size()-1)), state_map.get(seWS), seW, model.getProbability(current)));					
				}
				else {
					float weight = 0.0f;
					if (model.getBackoff(current) != 0) {
						weight = model.getBackoff(current);
					}
					new_state.addTransition(new Trans(new_state, state_map.get(current.getSubSequence(1, current.size())), epsW, weight));
					new_state.addTransition(new Trans(state_map.get(current.getSubSequence(0, current.size()-1)), new_state, current.getWord(current.size()-1), model.getProbability(current)));
				}
			}
			else if (current.size() == model.getMaxDepth()) {
				if (current.getWord(current.size()-1).equals(seW)) {
					Word[] temp = {current.getWord(current.size()-1)};
					WordSequence temp_seq = new WordSequence(temp);
					new_state.addTransition(new Trans(state_map.get(current.getSubSequence(0, current.size()-1)), state_map.get(temp_seq), current.getWord(current.size()-1), model.getProbability(current)));
				}
				else {
					new_state.addTransition(new Trans(state_map.get(current.getSubSequence(0, current.size()-1)), state_map.get(current.getSubSequence(1, current.size())), current.getWord(current.size()-1), model.getProbability(current)));
				}
			}
		}
		
		out.close();
		isyms_out.close();
		ssyms_out.close();
		
	}
	
	private void allocate(URL lm_location, URL dict_location, URL filler_location) throws IOException {
		// set up dictionary and language model
		readDictionary(dict_location, filler_location);
		readLanguageModel(lm_location);
		NGrams = model.getNGrams();
		states = new LinkedList<State>();
		state_map = new HashMap<WordSequence, State>();
		FSA_output = new FileWriter("lm_fsa");
		out = new BufferedWriter(FSA_output);
		isyms = new FileWriter("lm_fsa_isyms");
		isyms_out = new BufferedWriter(isyms);
		ssyms = new FileWriter("lm_fsa_ssyms");
		ssyms_out = new BufferedWriter(ssyms);
		i = 1;
		j = 1;		
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
		LogMath logm = new LogMath((float)1.0001, true);
		model = new SimpleNGramModel(lm_location, dict, (float)0.7, logm, 3);
		
		model.allocate();

	}
}
