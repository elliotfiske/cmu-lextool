/*
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
import edu.cmu.sphinx.util.StringCustomise;
import edu.cmu.sphinx.util.StringErrorGenerator;

public class LongAudioAligner {
	public static void main(String Args[]) throws IOException {		
		// Read Configuration file
		ConfigurationManager cm = new ConfigurationManager("./src/config.xml");
		Recognizer recognizer = (Recognizer) cm.lookup("recognizer");
		AlignerGrammar grammar = (AlignerGrammar) cm.lookup("AlignerGrammar");
		// Read raw input transcription from file
		String input = "";
		String line;
		BufferedReader reader = new BufferedReader(new FileReader(
				"./resource/transcription/bc1.txt"));
		while ((line = reader.readLine()) != null) {
			input = input.concat(line + " ");
		}
		
		// Clean-up the file to be suitable for making grammar
		StringCustomise sc= new StringCustomise();
		input=sc.customise(input);
		
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
		dataSource.setAudioFile(new URL("file:./resource/wav/bc1.wav"), null);

		Result result;
		String untimed = "";
		System.out.print("Result :");
		while ((result = recognizer.recognize()) != null) {
			String resultText = result.getBestResultNoFiller();
			String timedResult = result.getTimedBestResult(false, true);
			untimed = untimed.concat(resultText + " ");
			//System.out.println(timedResult);
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
