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
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.large.LargeNGramModel;
import edu.cmu.sphinx.util.LogMath;



/**
 * @author Alexandru Tomescu
 *
 */
public class PostProcessing {
	
	static int maxSequenceSize = 11000;	
	static LargeNGramModel lm;
	static float[] commaProbability = new float[101], periodProbability = new float[101];
	
	/**
	 * @param args
	 * @throws ClassNotFoundException 
	 * @throws IOException 
	 */
	public static void main(String[] args) throws ClassNotFoundException, IOException {
		
		String text = null, lm_path = null, input_file = null;
		int stackSize = 100;
		
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("-input_file")) input_file = args[i+1];
			else if (args[i].equals("-lm")) lm_path = args[i+1];
		}
		
		if (args.length < 4 || input_file == null || lm_path == null) {
			System.out.println("Usage: sh ./postprocess.sh -input_file t -lm lm_path ");
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
			outputFile = new FileWriter(input_file.split(".input")[0] + ".output");
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedReader input = new BufferedReader(inputFile);
		BufferedWriter output = new BufferedWriter(outputFile);
		
		lm = new LargeNGramModel("", new URL("file:" + lm_path), 
				"file:logfile", 0, false, 3, 
				new LogMath(10f, false),
				null, false, 0.0f, 0.0, 0.7f, false);
		
		lm.allocate();
		
		readDistanceProbabilities();
		
		
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
				
				WordSequence currentWordSequence = currentSequence.getWordSequence();
				
				int currentSize = currentSequence.getSequenceNumber() + 1;
				
				// if the retrieved sequence is full-sized, add <PERIOD> </s> and keep the sequence with the 
				// biggest probability
				
				if (currentSize == inputWords.size() + 1) {

					WordSequence fullSentence = currentWordSequence.addWord(new Word("<PERIOD>", null, false), maxSequenceSize).addWord(new Word("</s>", null, false), maxSequenceSize);
					Sequence fullSentenceSequence = new Sequence(fullSentence, getWSProb(fullSentence, lm), currentSize, currentSequence);
				
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
				
				// in case the word isn't in the language model it's kept as lower-cased				
				if (!lm.hasWord(currentWordForms[0]) && !lm.hasWord(currentWordForms[1])){
	
					WordSequence previousWords = new WordSequence(currentSequence.getWords());
					WordSequence newSequence = previousWords.addWord(currentWord, maxSequenceSize);			
					
					Sequence unpunctuated = new Sequence(newSequence, getWSProb(newSequence, lm), currentSize, currentSequence);
					stacks.addSequence(unpunctuated);
					continue;
				}
				
				for (Word wordForm : currentWordForms) {
					// verify if the written form currentWord is in the LM
					
					if (lm.hasWord(wordForm)) {
						
						WordSequence commaWordSequence = currentWordSequence.addWord(punctuationMarks[0], maxSequenceSize);
						WordSequence periodWordSequence = currentWordSequence.addWord(punctuationMarks[1], maxSequenceSize);
							
						Sequence commaSequence = new Sequence(commaWordSequence, getWSProb(commaWordSequence, lm), currentSize, currentSequence);
						Sequence periodSequence = new Sequence(periodWordSequence, getWSProb(periodWordSequence, lm), currentSize, currentSequence);
						
						//WordSequence previousWords = new WordSequence(currentSequence.getWords());
						WordSequence newSequence = commaWordSequence.addWord(wordForm, maxSequenceSize);	
	
						Sequence unpunctuated = new Sequence(newSequence, getWSProb(newSequence, lm), currentSize, commaSequence);
						stacks.addSequence(unpunctuated);
						
						newSequence = periodWordSequence.addWord(wordForm, maxSequenceSize);
						
						unpunctuated = new Sequence(newSequence, getWSProb(newSequence, lm), currentSize, periodSequence);
						stacks.addSequence(unpunctuated);
						
						newSequence = currentWordSequence.addWord(wordForm, maxSequenceSize);
						
						unpunctuated = new Sequence(newSequence, getWSProb(newSequence, lm), currentSize, currentSequence);
						stacks.addSequence(unpunctuated);
						
					}
				}
			}
			
			output.write(formatOutput(finalSequence.getWordSequence()) + '\n');
		}
		
		output.close();
		input.close();
	} 
	
	/**
	 * Remove <s> and </s> and [] from Word object
	 * 
	 * @param output - the WordSequence outputed by the decoder
	 * @return a string with correct formatting
	 */
	static String formatOutput(WordSequence output) {
		
		String newOutput = "";
		
		for (Word w : output.getWords()) {
			if (!w.toString().equals("<s>") && !w.toString().equals("</s>")) {
				if (w.toString().equals("<COMMA>")) {
					newOutput += ", ";
				} else if (w.toString().equals("<PERIOD>")) {
					newOutput += ". ";
				} else {
					newOutput += w.toString() + " ";		
				}
			}
				
		}
		
		return newOutput;
	}
	
	/**
	 * Break a string into Word objects and put them into a WordSequence
	 * 
	 * @param sentence
	 * @return
	 */
	static WordSequence breakIntoWords(String sentence) {
		LinkedList<Word> list = new LinkedList<Word>();
		
		String[] sWords = sentence.split(" ");
		
		for (String word : sWords) {
			list.add(new Word(word, null, false));
		}
		
		WordSequence words = new WordSequence(list);
		
		return words;
	}
	
	/**
	 * Evaluate the probability of a sentence
	 * 
	 * @param s
	 * @param lm
	 * @return
	 */
	static float evaluateSentence(String s, LargeNGramModel lm) {
		float prob = 0;
		WordSequence sentence = breakIntoWords(s);

		for (int i = 1; i <= sentence.size(); i++) {
			prob += getWSProb(sentence.getSubSequence(0, i), lm);
		}
		
		return prob + 99;
	}
	
	/**
	 * Get the probability of a WordSequence based on the language model
	 * 
	 * @param ws 
	 * @param lm - language model used to get the probability of the sequence
	 * @return - probability 
	 */
	public static float getWSProb(WordSequence ws, LargeNGramModel lm) {
		
		int[] punctuationDistance = getPunctuationDistance(ws);
		int commaDistance = punctuationDistance[0], periodDistance = punctuationDistance[1];
		
		if (ws.size() > 3) {
			ws = ws.getSubSequence(ws.size() - 3, ws.size());
		}
		
		float prob = 0;
		
		ArrayList<Word> words = new ArrayList<Word>();
		
		for (Word w : ws.getWords()) {
			if (lm.hasWord(w)) {
				words.add(w);
			}
		}

		WordSequence trimmedWS = new WordSequence(words);
		
		if (trimmedWS.size() > 0)		
			prob = lm.getProbability(trimmedWS);
		
		if (periodDistance <= 100) {
			if (periodProbability[periodDistance] != 0)
				prob += 0 * Math.log10(periodProbability[periodDistance]);
		} else prob = 0;
		

		if (commaDistance <= 100) {
			if (commaProbability[commaDistance] != 0)
				prob += 0 * Math.log10(commaProbability[commaDistance]);
		} else prob = 0;
			
		return prob;
	}
	
	/**
	 * Returns the distance from the last comma or period 
	 * @param ws - the word sequence that is evaluated
	 * @return - Integer array containing commaDistance and periodDistance
	 */
	public static int[] getPunctuationDistance(WordSequence ws) {
		int commaDistance = ws.size(), periodDistance = ws.size();
		for (int i = ws.size() - 1; i >= 0; i--) {
			if (ws.getWord(i).toString().equals("<COMMA>")) {
				commaDistance = ws.size() - i - 1;
			}
			else if (ws.getWord(i).toString().equals("<PERIOD>")) {
				periodDistance = ws.size() - i - 1;
			}
		}
		
		int[] p = {commaDistance, periodDistance};
		return p;
	}
	
	/**
	 * Read comma and period distance probabilities from the distance_probabilities file
	 * @throws IOException
	 */
	public static void readDistanceProbabilities() throws IOException {
		
		FileReader f = null;
		String line = null;
		int aux = 0;
		
		try {
			f = new FileReader("distance_probabilities");
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedReader input = new BufferedReader(f);
		
		while ((line = input.readLine()) != null) {
			if (line.equals("COMMA")) {
				aux = 0;
			}
			else if (line.equals("PERIOD")) {
				aux = 1;
			}
			else {
				if (aux == 0) {
					commaProbability[Integer.parseInt(line.split("\t")[0])] = Float.parseFloat(line.split("\t")[1]);
				}
				else {
					periodProbability[Integer.parseInt(line.split("\t")[0])] = Float.parseFloat(line.split("\t")[1]);
				}
			}
			
		}

	}
}
