package edu.cmu.sphinx.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Random;

public class StringErrorGenerator {

	private String text = null;
	private double wer = 0.03;
	private double ir = 0.01; // insertion rate (default= 1%)
	private double dr = 0.01; // deletion rate (default= 1%)
	private double sr = 0.01; // substitution rate (default= 1%)
	private Random rand;
	private List<String> wordsToInsert; // Contains words that will be inserted
	// or substituted.

	private int numWords = 0; // total number of words in the text
	private LinkedList<Word> words;
	private URL pathToWordFile;

	public StringErrorGenerator() {

	}

	/*
	 * Divides the input word error rate equally into insertions, deletions and
	 * substitution rates.
	 */
	public StringErrorGenerator(double wer, URL pathToWordFile) {
		this.wer = wer;
		this.ir = wer / 3;
		this.dr = wer / 3;
		this.sr = wer / 3;
		this.pathToWordFile = pathToWordFile;
	}

	// intialise un-equal error rates
	public StringErrorGenerator(double ir, double dr, double sr,
			URL pathToWordFile) {
		this.wer = ir + dr + sr;
		this.ir = ir;
		this.dr = dr;
		this.sr = sr;
		this.pathToWordFile = pathToWordFile;
	}

	// set Text to be corrupted
	public void setText(String text) {
		this.text = text;
	}

	/*
	 * Allocates Error Generator by assigning text dependent variables. Throws
	 * error when text is not set
	 */
	public void process() throws IOException {
		rand = new Random();
		if (text != null) {
			String[] wordTokens = text.split(" ");
			words = new LinkedList<Word>();
			for (int i = 0; i < wordTokens.length; i++) {
				if (wordTokens[i].compareTo("") != 0) {
					words.add(new Word(wordTokens[i]));
				}
			}
			numWords = words.size();
			// Load words to inserted from word file
			BufferedReader reader = new BufferedReader(new InputStreamReader(
					pathToWordFile.openStream()));
			String line;
			wordsToInsert = new LinkedList<String>();
			while ((line = reader.readLine()) != null) {
				wordsToInsert.add(line);
			}

		} else {
			throw new Error("ERROR: Can not allocate on a <null> text. ");
		}
		// Check for compatible word error rates
		check_compatible();
		
		// process errors
		processDeletions();
		processInsertions();
		processSubstitution();
	}
	
	// Throws error if error rates exceed acceptable bounds
	private void check_compatible() {
		if( wer > 1.0 || wer < 0) {
			throw new Error("Error: wer should be between 0 and 1.0");
		} else if (ir > 1.0 || ir < 0 ||
				   dr >1.0  || dr < 0 ||
				   sr >1.0  || sr < 0) {
			throw new Error("Error: insertion/deletion/substitution rates must be b/w 0 and 1.0");
		}
		
	}

	private void processSubstitution() {
		double numSubstitutions = sr * numWords;
		int substitutionCount = 0;
		int currIndex = 0;
		Iterator<Word> iter = words.listIterator(0);
		// while number of substitution is less than total number of required
		// substitutioniterate over the list and substitute word at random
		// locations with another one.
		while (substitutionCount < numSubstitutions) {
			if (currIndex < words.size()) {
				double random = rand.nextGaussian();
				if (random <= sr / 2 && random >= -sr / 2) {
					// Substitute a word here
					words.get(currIndex).delete();
					String wordToInsert= wordsToInsert.get(rand
							.nextInt(wordsToInsert.size()));
					Word word = new Word(wordToInsert);
					word.insert();
					words.add(currIndex,word);
					iter = words.listIterator(currIndex);
					substitutionCount++;
					currIndex--;
				}
				currIndex++;
			} else {
				// if current index has exceeded the total number of words,
				// start over again
				iter = words.listIterator(0);
				currIndex = 0;
			}
		}

	}

	/*
	 * Deletes words from random locations such that the total number of
	 * deletions equals the specified number.
	 */
	private void processDeletions() {
		double numDeletions = dr * numWords;
		int deletionCount = 0;
		int currIndex = 0;
		Iterator<Word> iter = words.listIterator(0);
		// while number of deletions is less than total number of required
		// deletions
		// iterate over the list and delete word from random locations.
		while (deletionCount < numDeletions) {
			if (currIndex < words.size()) {
				double random = rand.nextGaussian();
				if (random <= dr / 2 && random >= -dr / 2) {
					// Delete word from here
					words.get(currIndex).delete();
					iter = words.listIterator(currIndex);
					deletionCount++;
					currIndex--;
				}
				currIndex++;
			} else {
				// if current index has exceeded the total number of words,
				// start over again
				iter = words.listIterator(0);
				currIndex = 0;
			}
		}

	}

	/*
	 * Inserts new words at random locations such that the total number of
	 * insertions equals the specified number.
	 */
	private void processInsertions() {
		double numInsertions = ir * numWords;
		int insertionCount = 0;
		int currIndex = 0;
		Iterator<Word> iter = words.iterator();
		// while number of insertions is less than total number of required
		// insertions iterate over the list and insert random word at random
		// locations.
		while (insertionCount < numInsertions) {
			if (currIndex < words.size()) {
				double random = rand.nextGaussian();
				if (random <= ir / 2 && random >= -ir / 2) {
					// Insert a new word here
					String wordToInsert= wordsToInsert.get(rand
							.nextInt(wordsToInsert.size()));
					Word word = new Word(wordToInsert);
					word.insert();
					words.add(currIndex, word );
					iter = words.listIterator(currIndex);
					insertionCount = insertionCount + 1;
				}
				iter.next();
				currIndex++;
			} else {
				// if current index has exceeded the total number of words,
				// start over again
				iter = words.listIterator(0);
				currIndex = 0;
			}
		}
	}
	public LinkedList<Word> getTranscription() {
		return words;
	}
	
}
