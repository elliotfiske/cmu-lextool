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

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.URL;
import java.util.LinkedList;

import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.acoustic.UnitManager;
import edu.cmu.sphinx.linguist.dictionary.FastDictionary;
import edu.cmu.sphinx.linguist.dictionary.FullDictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.SimpleNGramModel;
import edu.cmu.sphinx.linguist.language.ngram.large.LargeNGramModel;
import edu.cmu.sphinx.util.LogMath;
import weka.classifiers.Classifier;
import weka.classifiers.trees.J48;
import weka.classifiers.trees.REPTree;
import weka.core.Instances;



/**
 * @author Alexandru Tomescu
 *
 */
public class PostProcessing {
	
	static int maxSequenceSize = 1000;	
	static LargeNGramModel lm;
	
	public static float getWSProb(WordSequence ws, LargeNGramModel lm) {
		if (ws.size() > 3) {
			ws = ws.getSubSequence(ws.size() - 3, ws.size());
		}
		return lm.getProbability(ws);
	}

	/**
	 * @param args
	 * @throws ClassNotFoundException 
	 * @throws IOException 
	 */
	public static void main(String[] args) throws ClassNotFoundException, IOException {
		
		String text = null, lm_path = null;
		
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("-text")) text = args[i+1].toLowerCase();
			else if (args[i].equals("-lm")) lm_path = args[i+1];
		}
		
		if (args.length < 4 || text == null || lm_path == null) {
			System.out.println("Usage: sh ./postp.sh -text t -lm lm_path ");
			return;
		}
		
		float max = Integer.MIN_VALUE;
		Sequence finalSequence = null;
		
		// load dictionary and language model
		FullDictionary dict = new FullDictionary(
				new URL("file:models/lm_giga_5k_nvp.sphinx.dic"), 
				new URL("file:models/lm_giga_5k_nvp.sphinx.filler"),
				null, false, null, true, true, new UnitManager());
		
		dict.allocate();
		
		lm = new LargeNGramModel("", new URL("file:" + lm_path), 
				"file:logfile", 0, false, 3, 
				new LogMath((float)10, false),
				dict, false, 0.0f, 0.0, 0.7f, false);
		
		lm.allocate();
		
		
		
		// put the words in the text into a Word Sequence
		WordSequence inputWords = breakIntoWords(text);
		
		// consider <s> the first symbol
		Word[] temp = {new Word("<s>", null, false)};
		Sequence firstSymbol = new Sequence(new WordSequence(temp), getWSProb(new WordSequence(temp), lm), -1);
		
		SequenceStack stack = new SequenceStack(10000);
		stack.addSequence(firstSymbol);
		
		while (!stack.isEmpty()) {
			// retrieve the first sequence in the stack
			Sequence h = stack.getSequence();

			int current = h.getSequenceNumber() + 1;
			
			// if the retrieved sequence is full-sized, add </s> and keep the sequence with the 
			// biggest probability
			if (current == inputWords.size()) {
				WordSequence fullSentence = h.getWordSequence().addWord(new Word("</s>", null, false), maxSequenceSize);
				
				Sequence fullSentenceSymbol = new Sequence(fullSentence, getWSProb(fullSentence, lm), current);
				if (fullSentenceSymbol.getProbability() > max) {
					finalSequence = fullSentenceSymbol;
					max = fullSentenceSymbol.getProbability();
				}
				continue;
			}
			
			// get the next word that needs to be added and compute it's written forms
			Word currentWord = inputWords.getWord(current);
			
			Word[] currentWordForms = {currentWord, 
					new Word(WordUtils.capitalize(currentWord.toString()), null, false),
					new Word(currentWord.toString().toUpperCase(), null, false)}; 
			
			Word[] punctuationMarks = {new Word("<NONE>", null, false), 
					new Word("<COMMA>", null, false),
					new Word("<PERIOD>", null, false)}; 
			
			for (Word wordForm : currentWordForms) {
				// verify if the written form currentWord is in the LM
				if (lm.hasUnigram(wordForm)) {
					WordSequence previousWords = new WordSequence(h.getWords());	
					WordSequence newSequence = previousWords.addWord(wordForm, maxSequenceSize);			
					
					for (Word punctuation : punctuationMarks) {
						WordSequence punctSequence = newSequence.addWord(punctuation, maxSequenceSize);
						
						Sequence newSequenceHistory = new Sequence(punctSequence, getWSProb(newSequence, lm), current);
						stack.addSequence(newSequenceHistory);
					}
				}
			}
		}
		
		System.out.println(formatOutput(finalSequence.getWordSequence()));
	} 
	
	
	
	
	
	static String formatOutput(WordSequence output) {
		
		String newOutput = "";
		
		for (Word w : output.getWords()) {
			if (w.toString().equals("<NONE>")) {
				newOutput += " ";
			} else if (w.toString().equals("<PERIOD>")) {
				newOutput += ". ";
			} else if (w.toString().equals("<COMMA>")) {
				newOutput += ", ";
			} else {
				newOutput += w.toString();
			}
			
		}
		
		return newOutput;
	}
	
	static WordSequence breakIntoWords(String sentence) {
		LinkedList<Word> list = new LinkedList<Word>();
		
		String[] sWords = sentence.split(" ");
		
		for (String word : sWords) {
			list.add(new Word(word, null, false));
		}
		
		WordSequence words = new WordSequence(list);
		
		return words;
	}
}
