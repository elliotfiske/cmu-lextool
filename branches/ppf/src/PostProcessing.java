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

import java.io.IOException;
import java.util.LinkedList;

import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.acoustic.UnitManager;
import edu.cmu.sphinx.linguist.dictionary.FastDictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.SimpleNGramModel;
import edu.cmu.sphinx.util.LogMath;


/**
 * @author Alexandru Tomescu
 *
 */
public class PostProcessing {
	
	static int maxSequenceSize = 1000;	
	static SimpleNGramModel lm;

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
		
		float max = -10000;
		Sequence finalSequence = null;
		
		// load dictionary and language model		
		FastDictionary dict = new FastDictionary("../models/lm_giga_5k_nvp.sphinx.dic", 
				"../models/lm_giga_5k_nvp.sphinx.filler",
				null, false, "<sil>", true, false, new UnitManager());
		dict.allocate();
		
		lm = new SimpleNGramModel(lm_path, dict, 0.7f, 
				new LogMath(10, false), 3);
		lm.allocate();
		
		// put the words in the text into a Word Sequence
		WordSequence inputWords = breakIntoWords(text);
		
		// consider <s> the first symbol
		Word[] temp = {new Word("<s>", null, false)};
		Sequence firstSymbol = new Sequence(new WordSequence(temp), lm.getProbability(new WordSequence(temp)), -1);
		
		SequenceStack stack = new SequenceStack(10000);
		stack.addSequence(firstSymbol);
		
		while (!stack.isEmpty()) {
			Sequence h = stack.getSequence();
			
			int current = h.getSequenceNumber() + 1;
			
			if (current == inputWords.size()) {
				WordSequence fullSentence = h.getWordSequence().addWord(new Word("</s>", null, false), maxSequenceSize);
				
				Sequence fullSentenceSymbol = new Sequence(fullSentence, lm.getProbability(fullSentence), current);
				if (fullSentenceSymbol.getProbability() > max) {
					finalSequence = fullSentenceSymbol;
					max = fullSentenceSymbol.getProbability();
				}
				continue;
			}
			
			Word currentWord = inputWords.getWord(current);
			
			
			Word[] currentWordForms = {currentWord, 
					new Word(WordUtils.capitalize(currentWord.toString()), null, false),
					new Word(currentWord.toString().toUpperCase(), null, false)}; 
			
			Word[] punctuationMarks = {new Word("<NONE>", null, false), 
					new Word("<COMMA>", null, false),
					new Word("<PERIOD>", null, false)}; 
			
			for (Word wordForm : currentWordForms) {
				WordSequence previousWords = new WordSequence(h.getWords());				
				WordSequence newSequence = previousWords.addWord(wordForm, maxSequenceSize);				
				
				for (Word punctuation : punctuationMarks) {
					WordSequence punctSequence = newSequence.addWord(punctuation, maxSequenceSize);
					
					Sequence newSequenceHistory = new Sequence(punctSequence, lm.getProbability(newSequence), current);
					stack.addSequence(newSequenceHistory);
				}
			}
		}
		
		System.out.println(formatOutput(finalSequence.getWordSequence()) + " ---- with probability " + finalSequence.getProbability());
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
