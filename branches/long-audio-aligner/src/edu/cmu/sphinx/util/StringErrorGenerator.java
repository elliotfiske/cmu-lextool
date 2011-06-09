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
	private List<String> words;
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
	public void allocate() throws IOException {
		rand = new Random();
		if (text != null) {
			String[] wordTokens = text.split(" ");
			words = new LinkedList<String>();
			for (int i = 0; i < wordTokens.length; i++) {
				if (wordTokens[i].compareTo("") != 0) {
					words.add(wordTokens[i]);
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
	}

	public String generateTranscription() {
		processDeletions();
		processInsertions();
		processSubstitution();
		Iterator<String> iter = words.iterator();
		String result = "";
		while (iter.hasNext()) {
			result = result.concat(iter.next() + " ");
		}
		// System.out.println(result);
		return result;
	}

	private void processSubstitution() {
		double numSubstitutions = sr * numWords;
		int substitutionCount = 0;
		int currIndex = 0;
		Iterator<String> iter = words.listIterator(0);
		// while number of substitution is less than total number of required
		// substitutioniterate over the list and substitute word at random
		// locations with another one.
		while (substitutionCount < numSubstitutions) {
			if (currIndex < words.size()) {
				double random = rand.nextGaussian();
				if (random <= sr / 2 && random >= -sr / 2) {
					// Substitute a word here
					words.remove(currIndex);
					words.add(currIndex, wordsToInsert.get(rand
							.nextInt(wordsToInsert.size())));
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
		Iterator<String> iter = words.listIterator(0);
		// while number of deletions is less than total number of required
		// deletions
		// iterate over the list and delete word from random locations.
		while (deletionCount < numDeletions) {
			if (currIndex < words.size()) {
				double random = rand.nextGaussian();
				if (random <= dr / 2 && random >= -dr / 2) {
					// Delete word from here
					System.out.println("Removing word: "
							+ words.remove(currIndex));
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
		Iterator<String> iter = words.iterator();
		// while number of insertions is less than total number of required
		// insertions iterate over the list and insert random word at random
		// locations.
		while (insertionCount < numInsertions) {
			if (currIndex < words.size()) {
				double random = rand.nextGaussian();
				if (random <= ir / 2 && random >= -ir / 2) {
					// Insert a new word here
					words.add(currIndex, wordsToInsert.get(rand
							.nextInt(wordsToInsert.size())));
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
}
