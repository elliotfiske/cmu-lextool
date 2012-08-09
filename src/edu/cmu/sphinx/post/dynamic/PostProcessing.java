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
package edu.cmu.sphinx.post.dynamic;
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.ArrayList;
import java.util.LinkedList;

import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.acoustic.UnitManager;
import edu.cmu.sphinx.linguist.dictionary.FullDictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.large.LargeNGramModel;
import edu.cmu.sphinx.util.LogMath;



/**
 * @author Alexandru Tomescu
 *
 */
public class PostProcessing {
	
	static int maxSequenceSize = 1000;	
	static LargeNGramModel lm;
	
	/**
	 * @param args
	 * @throws ClassNotFoundException 
	 * @throws IOException 
	 */
	public static void main(String[] args) throws ClassNotFoundException, IOException {
		
		String text = null, lm_path = null, input_file = null;
		int stackSize = 10000;
		
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("-input_file")) input_file = args[i+1];
			else if (args[i].equals("-lm")) lm_path = args[i+1];
		}
		
		if (args.length < 4 || input_file == null || lm_path == null) {
			System.out.println("Usage: sh ./postp.sh -input_file t -lm lm_path ");
			return;
		}
		
		FileReader inputFile = null;
		FileWriter outputFile = null;
		
		try {
			inputFile = new FileReader(input_file);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			outputFile = new FileWriter("output");
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedReader input = new BufferedReader(inputFile);
		BufferedWriter output = new BufferedWriter(outputFile);
		
		
		// load dictionary and language model
		FullDictionary dict = new FullDictionary(
				new URL("file:models/lm_giga_5k_nvp.sphinx.dic"), 
				new URL("file:models/lm_giga_5k_nvp.sphinx.filler"),
				null, false, null, true, true, new UnitManager());
		
		dict.allocate();
		
		lm = new LargeNGramModel("", new URL("file:" + lm_path), 
				"file:logfile", 0, false, 3, 
				new LogMath(10f, false),
				dict, false, 0.0f, 0.0, 0.7f, false);
		
		lm.allocate();
		
		
		while ((text = input.readLine()) != null) {
			float max = Integer.MIN_VALUE;
			Sequence finalSequence = null;
			StackCollection stacks = new StackCollection(stackSize);
		
			// put the words in the text into a Word Sequence
			WordSequence inputWords = breakIntoWords(text.replaceAll("\\s+", " "));			
			
			// consider <s> the first symbol
			Word[] temp = {new Word("<s>", null, false)};
			Sequence firstSymbol = new Sequence(new WordSequence(temp), 0f, 0, null);
			
			stacks.addSequence(firstSymbol);
	
			while (!stacks.isEmpty()) {
				// retrieve the first sequence in the stack
				Sequence currentSequence = stacks.getSequence();
				
				//System.out.println(currentSequence);
				WordSequence currentWordSequence = currentSequence.getWordSequence();
				int currentSize = currentSequence.getSequenceNumber() + 1;
				
				// if the retrieved sequence is full-sized, add </s> and keep the sequence with the 
				// biggest probability
				
				if (currentSize == inputWords.size() + 1) {

					WordSequence fullSentence = currentWordSequence.addWord(new Word("</s>", null, false), maxSequenceSize);
					Sequence fullSentenceSequence = new Sequence(fullSentence, getWSProb(fullSentence, lm), currentSize, currentSequence);
					
					//output.write(formatOutput(fullSentenceSequence.getWordSequence()) + " " + fullSentenceSequence.getProbability() + "\n");
				
					if (fullSentenceSequence.getProbability() > max) {
						finalSequence = fullSentenceSequence;
						max = fullSentenceSequence.getProbability();
					}
					continue;
				}
				
				// get the next word that needs to be added and compute it's written forms
				Word currentWord = inputWords.getWord(currentSize - 1);
				
				Word[] currentWordForms = {currentWord, 
						new Word(WordUtils.capitalize(currentWord.toString()), null, false)}; 
				
				Word[] punctuationMarks = {new Word("<COMMA>", null, false),
						new Word("<PERIOD>", null, false)}; 
				
				if (!lm.hasUnigram(currentWordForms[0]) && !lm.hasUnigram(currentWordForms[1])){
	
					WordSequence previousWords = new WordSequence(currentSequence.getWords());
					WordSequence newSequence = previousWords.addWord(currentWord, maxSequenceSize);			
					
	
					Sequence unpunctuated = new Sequence(newSequence, getWSProb(newSequence, lm), currentSize, currentSequence);
					stacks.addSequence(unpunctuated);
					continue;
				}
				
				for (Word wordForm : currentWordForms) {
					// verify if the written form currentWord is in the LM
					
					if (lm.hasUnigram(wordForm)) {
						WordSequence previousWords = new WordSequence(currentSequence.getWords());
						WordSequence newSequence = previousWords.addWord(wordForm, maxSequenceSize);	
	
						Sequence unpunctuated = new Sequence(newSequence, getWSProb(newSequence, lm), currentSize, currentSequence);
						stacks.addSequence(unpunctuated);
						
						WordSequence commaSequence = newSequence.addWord(punctuationMarks[0], maxSequenceSize);
						WordSequence periodSequence = newSequence.addWord(punctuationMarks[1], maxSequenceSize);
							
						Sequence newSequenceHistory = new Sequence(commaSequence, getWSProb(commaSequence, lm), currentSize, unpunctuated);
						stacks.addSequence(newSequenceHistory);
						newSequenceHistory = new Sequence(periodSequence, getWSProb(periodSequence, lm), currentSize, unpunctuated);
						stacks.addSequence(newSequenceHistory);
					}
				}
			}
			
			output.write(formatOutput(finalSequence.getWordSequence()) + '\n');
			
			System.out.println(formatOutput(finalSequence.getWordSequence()) + " " + finalSequence.getProbability() + '\n' );
			
		}
		
		//System.out.println(evaluateSentence("<s> I saw him <COMMA> putting a razor edge <COMMA> on his bayonet <PERIOD> Last night <COMMA> added Bart <PERIOD> Now <COMMA> he's anxious to see how it works <PERIOD> He'll have plenty of chances <COMMA> to find out <COMMA> said Frank <PERIOD> This is going to be a hot <COMMA> scrap <COMMA> or I miss my guess <PERIOD> </s>", lm));
		//System.out.println(evaluateSentence("<s> I saw him putting a razor edge on his bayonet last night <COMMA> added Bart <PERIOD>  Now he's anxious to see how it works <PERIOD>  He'll have plenty of chances to find out <COMMA> said Frank <PERIOD>  This is going to be a hot scrap <COMMA> or I miss my guess <PERIOD> </s>", lm));

		
		output.close();
		input.close();
	} 
	
	static String formatOutput(WordSequence output) {
		
		String newOutput = "";
		
		for (Word w : output.getWords()) {
			if (!w.toString().equals("<s>") && !w.toString().equals("</s>"))
				newOutput += w.toString() + " ";			
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
	
	static float evaluateSentence(String s, LargeNGramModel lm) {
		float prob = 0;
		WordSequence sentence = breakIntoWords(s);

		for (int i = 1; i <= sentence.size(); i++) {
			prob += getWSProb(sentence.getSubSequence(0, i), lm);
			System.out.println(formatOutput(sentence.getSubSequence(0, i)) + " " + getWSProb(sentence.getSubSequence(0, i), lm));
		}
		
		return prob + 99;
	}
	
	public static float getWSProb(WordSequence ws, LargeNGramModel lm) {
		
		if (ws.size() > 3) {
			ws = ws.getSubSequence(ws.size() - 3, ws.size());
		}
		
		float prob = 0;
		
		ArrayList<Word> words = new ArrayList<Word>();
		
		for (Word w : ws.getWords()) {
			if (lm.hasUnigram(w)) {
				words.add(w);
			}
		}

		WordSequence trimmedWS = new WordSequence(words);
		
		prob = lm.getProbability(trimmedWS);
		
		//System.out.println(trimmedWS);
		//System.out.println(trimmedWS.getSubSequence(trimmedWS.size()-1, trimmedWS.size()).toString());
		
		/*if (trimmedWS.getSubSequence(trimmedWS.size()-1, trimmedWS.size()).toString().equals("[<COMMA>]")) {
			prob *= 4;
			//System.out.println(trimmedWS);
		} else if (trimmedWS.getSubSequence(trimmedWS.size()-1, trimmedWS.size()).toString().equals("[<PERIOD>]")) {
			prob *= 3;
		}*/
		
		return prob;
	}
}
