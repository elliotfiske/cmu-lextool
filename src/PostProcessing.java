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
import java.net.MalformedURLException;
import java.net.URL;
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
		
		String text = args[0].toLowerCase();
		String modelPath = args[1];
		float max = -10000;
		Sequence finalSequence = null;
		
		// load dictionary and language model		
		FastDictionary dict = new FastDictionary("models/lm_giga_5k_nvp.sphinx.dic", 
				"models/lm_giga_5k_nvp.sphinx.filler",
				null, false, "<sil>", true, false, new UnitManager());
		dict.allocate();
		
		lm = new SimpleNGramModel(modelPath, dict, 0.7f, 
				new LogMath(10, false), 3);
		lm.allocate();
		
		// put the words in the text into a Word Sequence
		WordSequence inputWords = breakIntoWords(text);
				
		LinkedList<Sequence> sequences = new LinkedList<Sequence>();
		
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
		
		System.out.println(finalSequence.toString() + " with probability " + finalSequence.getProbability());
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
