/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.demo.phrasespotting;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.ListIterator;
import java.util.StringTokenizer;

import edu.cmu.sphinx.decoder.search.Token;
import edu.cmu.sphinx.frontend.FloatData;
import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.linguist.SearchState;
import edu.cmu.sphinx.linguist.UnitSearchState;
import edu.cmu.sphinx.linguist.WordSearchState;
import edu.cmu.sphinx.linguist.acoustic.Unit;
import edu.cmu.sphinx.linguist.aflat.AFlatLinguist;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;
import edu.cmu.sphinx.phrasespotter.Result;
import edu.cmu.sphinx.phrasespotter.simplephrasespotter.SimplePhraseSpotter;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.AlignerResult;
import edu.cmu.sphinx.util.StringCustomise;
import edu.cmu.sphinx.util.props.ConfigurationManager;

public class PhraseSpotting {
	public static void main(String Args[]) throws IOException {

		// Initialise demo related variables
		final String phrase = "quantum number"; // Phrase to be spotted
		String pathToAudioFile = "./resource/wav/test.wav"; // Audio file
		String pathToTextFile = "./resource/Transcription/test.txt"; // Transcription
		// file

		System.out.println("Phrase: " + phrase);
		ConfigurationManager cm = new ConfigurationManager("./src/config.xml");
		Recognizer recognizer = (Recognizer) cm.lookup("recognizer");
		AFlatLinguist aflatLinguist = (AFlatLinguist) cm.lookup("linguist");
		AlignerGrammar grammar = (AlignerGrammar) cm.lookup("grammar");
		AudioFileDataSource dataSource = (AudioFileDataSource) cm
				.lookup("audioFileDataSource");

		BufferedReader reader = new BufferedReader(new FileReader(
				pathToTextFile));
		String input = "";
		String line;
		while ((line = reader.readLine()) != null) {
			input = input.concat(line + " ");
		}
		StringCustomise sc = new StringCustomise();
		input = sc.customise(input);
		grammar.setGrammarType("");
		grammar.setText(input);
		dataSource.setAudioFile(new URL("file:" + pathToAudioFile), null);
		recognizer.allocate();

		System.out
				.println("-------------------- Generating Timed Result -----------------------");
		edu.cmu.sphinx.result.Result baseResult = recognizer.recognize();
		String timedResult = baseResult.getTimedBestResult(false, true);
		System.out.println(timedResult);
		AlignerResult alignerResult = new AlignerResult(baseResult);
		System.out.println(alignerResult.getBestTimedPhoneResult());
		
		System.out.println("Times when the Phrase \"" + phrase
				+ "\" was spoken:");
		List<String> wordsInPhrase = new LinkedList<String>();
		StringTokenizer tokenizer = new StringTokenizer(phrase);
		while (tokenizer.hasMoreTokens()) {
			wordsInPhrase.add(tokenizer.nextToken());
		}

		List<Result> baseTimedResult = new LinkedList<Result>();
		StringTokenizer stoken = new StringTokenizer(timedResult);
		while (stoken.hasMoreTokens()) {
			String wordFormed = "";
			String currWord = stoken.nextToken();
			String word = currWord.substring(0, currWord.indexOf("("));
			String timedPart = currWord.substring(currWord.indexOf("(") + 1,
					currWord.indexOf(")"));
			String st = timedPart.substring(0, timedPart.indexOf(","));
			String et = timedPart.substring(timedPart.indexOf(",") + 1);
			ListIterator<String> iter = wordsInPhrase.listIterator();
			String phraseWord = iter.next();
			float startTime = Float.valueOf(st);
			float endTime = Float.valueOf(et);
			int match = 0;
			while (phraseWord.compareToIgnoreCase(word) == 0
					&& match < wordsInPhrase.size()) {
				match++;
				wordFormed += word + " ";
				endTime = Float.valueOf(et);
				if (stoken.hasMoreTokens()) {
					if (iter.hasNext()) {
						phraseWord = iter.next();
						currWord = stoken.nextToken();
						word = currWord.substring(0, currWord.indexOf("("));
						timedPart = currWord.substring(
								currWord.indexOf("(") + 1, currWord
										.indexOf(")"));
						st = timedPart.substring(0, timedPart.indexOf(","));
						et = timedPart.substring(timedPart.indexOf(",") + 1);
					}
				}
			}
			// if phrase was completely detected then mark this position

			if (match == wordsInPhrase.size()) {
				System.out.println("(" + startTime + "," + endTime + ")");
				baseTimedResult.add(new Result(phrase, startTime, endTime));
			}
		}

		// Start PhraseSpotting now and see how well it performs

		System.out
				.println("\n------------- Generating Phrase Spotter's Result --------------------");
		SimplePhraseSpotter spotter = new SimplePhraseSpotter(
				"./src/phraseSpotterConfig.xml");
		spotter.setPhrase(phrase);
		spotter.setAudioDataSource(new URL("file:" + pathToAudioFile));
		spotter.allocate();
		spotter.startSpotting();
		List<Result> result = spotter.getTimedResult();
		Iterator<Result> iter = result.iterator();
		System.out.println("Times when \"" + phrase + "\" was spotted");
		while (iter.hasNext()) {
			Result data = iter.next();
			System.out.println("(" + data.getStartTime() + ","
					+ data.getEndTime() + ")");
		}

		int numCorrectSpottings = 0;
		Iterator<Result> baseIter = baseTimedResult.iterator();
		while (baseIter.hasNext()) {
			iter = result.iterator();
			Result currBaseToken = baseIter.next();

			while (iter.hasNext()) {
				Result currPhraseToken = iter.next();
				if (currBaseToken.equals(currPhraseToken) == 0) {
					numCorrectSpottings++;
					continue;
				}
			}
		}
		Float errorRate = (float) (baseTimedResult.size() - numCorrectSpottings)
				/ (float) baseTimedResult.size();
		String erToString = errorRate.toString();
		if (erToString.length() > 6) {
			erToString = erToString.substring(0, 6);
		}
		Float accuracy = ((float) numCorrectSpottings / (float) baseTimedResult
				.size());
		String accuracyToString = accuracy.toString();
		if (accuracyToString.length() > 6) {
			accuracyToString = accuracyToString.substring(0, 6);
		}
		System.out
				.println("------------------- Statistics ---------------------");
		System.out.println("Number of phrase occurances:\t\t\t"
				+ baseTimedResult.size());
		System.out.println("Number of phrases correctly spotted:\t\t"
				+ numCorrectSpottings);
		System.out.println("Number of uncaught phrase occurance:\t\t"
				+ (baseTimedResult.size() - numCorrectSpottings));
		System.out.println("Number of false alarms:\t\t\t\t"
				+ (result.size() - numCorrectSpottings));
		System.out.println("Error rate:\t\t\t\t\t" + erToString);
		System.out.println("Accuracy rate:\t\t\t\t\t" + accuracyToString);

	}
}
