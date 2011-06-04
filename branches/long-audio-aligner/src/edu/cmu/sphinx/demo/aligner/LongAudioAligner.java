package edu.cmu.sphinx.demo.aligner;

import java.util.List;
import java.io.BufferedReader;
import java.io.BufferedWriter;

import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import java.net.URL;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.StringTokenizer;
import java.util.Timer;

import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.linguist.language.grammar.TextAlignerGrammar;
import edu.cmu.sphinx.util.NISTAlign;

public class LongAudioAligner {
	public static void main(String Args[]) throws IOException {
		BufferedReader reader = new BufferedReader(new FileReader(
				"./resource/transcription/numbers.txt"));
		ConfigurationManager cm = new ConfigurationManager("./src/config.xml");
		Recognizer recognizer = (Recognizer) cm.lookup("recognizer");
		AlignerGrammar grammar = (AlignerGrammar) cm.lookup("AlignerGrammar");
		String tempInput = null;
		String input = "";

		while ((tempInput = reader.readLine()) != null) {
			input = input.concat(tempInput + " ");
		}
		String Input = "";
		StringTokenizer tok = new StringTokenizer(input, ".");
		while (tok.hasMoreTokens()) {
			Input = Input.concat(tok.nextToken() + " ");
		}
		System.out.println(Input);
		long startTime = System.currentTimeMillis();

		grammar.setText(Input);

		recognizer.allocate();

		AudioFileDataSource dataSource = (AudioFileDataSource) cm
				.lookup("audioFileDataSource");
		dataSource.setAudioFile(new URL("file:./resource/wav/numbers.wav"),
				null);
		Result result;
		String untimed = "";
		while ((result = recognizer.recognize()) != null) {
			String resultText = result.getBestFinalResultNoFiller();
			String timedResult = result.getTimedBestResult(false, true);
			untimed = untimed.concat(resultText + " ");
			System.out.println(timedResult);
		}		
		System.out.println("Time to align:"
				+ (System.currentTimeMillis() - startTime) / 1000 + "secs");

		NISTAlign nistalign = new NISTAlign(true, true);
		nistalign.align(Input, untimed);
		System.out.println("WER:" + nistalign.getTotalWordErrorRate());
		System.out.println("Total words:" + nistalign.getTotalWords());
		System.out.println("Word errors:" + nistalign.getTotalWordErrors());
		System.out.println("Insertions:" + nistalign.getTotalInsertions());
		System.out.println("Deletions:" + nistalign.getTotalDeletions());
		System.out.println("Subs:" + nistalign.getTotalSubstitutions());

	}

}
