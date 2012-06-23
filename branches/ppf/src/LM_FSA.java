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
public class LM_FSA {
	private LinkedList<State> states;
	private FastDictionary dict;
	private SimpleNGramModel model;
	LinkedList<WordSequence> NGrams;
	FileWriter file;
	BufferedWriter out;

	/**
	 * 
	 * @param lm_location - location of the language model
	 * @param dict_location - location of the dictionary
	 * @param filler_location -location of the filler dictionary
	 * @throws IOException
	 */
	public LM_FSA(URL lm_location, URL dict_location, URL filler_location) throws IOException {
		
		states = new LinkedList<State>();
		
		readDictionary(dict_location, filler_location);
		
		readLanguageModel(lm_location);
		
		NGrams = model.getNGrams();
		
		file = new FileWriter("NGrams");
		
		out = new BufferedWriter(file);
		
		out.write(NGrams.toString());
		
		out.close();
		
		State firstState = new State();
		
		states.add(firstState);
		
		Iterator<WordSequence> it = NGrams.iterator();
		
		while (it.hasNext()) {
			WordSequence current = it.next();
			
			addNGram(current, 0, firstState);
		}
		
	}
	
	private void addNGram(WordSequence words, int word_id, State current_state) {
		
		Word current_word = words.getWord(word_id);
		State st = null;
		st = current_state.containsTransition(current_word);
		
		if (st == null) {
			System.out.print(words + " put after " + current_state.getWords() + " with probability ");
			addState(words, current_word, current_state);
			
		} else {
			addNGram(words, word_id + 1, st);
		}
	}
	
	private void addState(WordSequence words, Word word, State start_state) {
		
		State finish_state = new State(words);
		float probability = model.getProbability(words);
		Trans new_transition = new Trans(start_state, finish_state, word, probability);
		start_state.addTransition(new_transition);
		System.out.println(new_transition.getProbability() + '\n');
		
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
