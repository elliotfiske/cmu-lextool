package edu.cmu.sphinx.demo.aligner;

import java.awt.List;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import java.net.URL;

import java.util.LinkedList;
import java.util.StringTokenizer;

import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import edu.cmu.sphinx.frontend.util.AudioFileDataSource;

import edu.cmu.sphinx.util.NISTAlign;
import edu.cmu.sphinx.util.StringErrorGenerator;

public class LongAudioAligner {
	public static void main(String Args[]) throws IOException {
		// Read Configuration file
		ConfigurationManager cm = new ConfigurationManager("./src/config.xml");
		Recognizer recognizer = (Recognizer) cm.lookup("recognizer");
		AlignerGrammar grammar = (AlignerGrammar) cm.lookup("AlignerGrammar");

		// Read input transcription from transcription file
		String input = "";
		String line;
		BufferedReader reader = new BufferedReader(new FileReader(
				"./resource/transcription/black_cat1.txt"));
		while ((line = reader.readLine()) != null) {
			input = input.concat(line + " ");
		}
		StringTokenizer tok = new StringTokenizer(input, ".");
		input = "";
		while (tok.hasMoreTokens()) {
			String nextTok = tok.nextToken();
			input = input.concat(nextTok + " ");
		}
		// Corrupt the input using StringErrorGenerator
		URL pathToWordFile = new URL("file:./resource/models/wordFile.txt");
		StringErrorGenerator seg = new StringErrorGenerator(0.03,
				pathToWordFile);
		seg.setText(input);
		seg.allocate();
		String corruptedInput = seg.generateTranscription();
		long startTime = System.currentTimeMillis();
		grammar.setText(corruptedInput);
		recognizer.allocate();

		AudioFileDataSource dataSource = (AudioFileDataSource) cm
				.lookup("audioFileDataSource");
		dataSource.setAudioFile(new URL("file:./resource/wav/black_cat1.wav"), null);

		Result result;
		String untimed = "";
		while ((result = recognizer.recognize()) != null) {
			String resultText = result.getBestFinalResultNoFiller();
			String timedResult = result.getTimedBestResult(false, true);
			untimed = untimed.concat(resultText + " ");
			System.out.println(timedResult);
			System.out.println(untimed);
		}
		System.out.println("Time to align:"
				+ (System.currentTimeMillis() - startTime) / 1000 + "secs");

		NISTAlign nistalign = new NISTAlign(true, true);
		nistalign.align(input, untimed);
		System.out.println("WER:" + nistalign.getTotalWordErrorRate());
		System.out.println("Total words:" + nistalign.getTotalWords());
		System.out.println("Word errors:" + nistalign.getTotalWordErrors());
		System.out.println("Insertions:" + nistalign.getTotalInsertions());
		System.out.println("Deletions:" + nistalign.getTotalDeletions());
		System.out.println("Subs:" + nistalign.getTotalSubstitutions());

	}

}
