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
import java.util.Map;

import edu.cmu.sphinx.linguist.flat.FlatLinguist;
import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;
import edu.cmu.sphinx.linguist.language.grammar.TextAlignerGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.Configurable;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.frontend.util.AudioFileDataSource;

import edu.cmu.sphinx.util.AlignerTestCase;
import edu.cmu.sphinx.util.NISTAlign;
import edu.cmu.sphinx.util.StringCustomise;
import edu.cmu.sphinx.util.StringErrorGenerator;
import edu.cmu.sphinx.util.WordErrorCount;

public class LongAudioAligner {
	public static void main(String Args[]) throws IOException {
		// Read Configuration file
		ConfigurationManager cm = new ConfigurationManager("./src/config.xml");		
		Recognizer recognizer = (Recognizer) cm.lookup("recognizer");
		FlatLinguist flatLinguist = (FlatLinguist) cm.lookup("flatLinguist");
		AlignerGrammar grammar = (AlignerGrammar) cm.lookup("AlignerGrammar");
		grammar.setGrammarType("");
		
		// Read raw input transcription from file
		String input = "";
		String line;
		BufferedReader reader = new BufferedReader(new FileReader(
				"./resource/transcription/black_cat1.txt"));
		while ((line = reader.readLine()) != null) {
			input = input.concat(line + " ");
		}
		
		// Clean-up the file to be suitable for making grammar
		StringCustomise sc= new StringCustomise();
		input=sc.customise(input);
		
		// Corrupt the input using StringErrorGenerator
		URL pathToWordFile = new URL("file:./resource/models/wordFile.txt");
		AlignerTestCase testCase = new AlignerTestCase(input, 0.03, pathToWordFile);
		String corruptedInput = testCase.getCorruptedText();	
		// System.out.println("CorruptedInput="+corruptedInput);
		grammar.setText(corruptedInput);
		
		recognizer.allocate();

		AudioFileDataSource dataSource = (AudioFileDataSource) cm
				.lookup("audioFileDataSource");
		
		dataSource.setAudioFile(new URL("file:./resource/wav/black_cat1.wav"), null);
		
		Result result;
		String untimed = "";
		//System.out.print("Original Aligner :");
		
		while ((result = recognizer.recognize()) != null) {
			String resultText = result.getBestResultNoFiller();
			String timedResult = result.getTimedBestResult(false, true);
			untimed = untimed.concat(resultText + " ");
		}
		
		NISTAlign nistalign = new NISTAlign(true, true);
		nistalign.align(input, untimed);
		System.out.println("=======Force Aligned=========");
		System.out.println("Deletions:"+nistalign.getTotalDeletions());
		System.out.println("Insertions:"+nistalign.getTotalInsertions());
		System.out.println("Substitution:"+nistalign.getTotalSubstitutions());
		
		WordErrorCount wec = new WordErrorCount(testCase.getWordList(), untimed);
		wec.align();
		wec.printStats();
		
		flatLinguist.deallocate();
		// change grammar Configurations
		System.out.println("=======Modified Grammar======");
		grammar.setGrammarType("MODEL_BACKWARD_JUMPS|MODEL_REPETITIONS");
		flatLinguist.allocate();
		dataSource.setAudioFile(new URL("file:./resource/wav/black_cat1.wav"), null);
		untimed = "";
		while ((result = recognizer.recognize()) != null) {
			String resultText = result.getBestResultNoFiller();
			String timedResult = result.getTimedBestResult(false, true);
			untimed = untimed.concat(resultText + " ");
		}
		nistalign.align(input, untimed);
		System.out.println("WER:" + nistalign.getTotalWordErrorRate());
	}		

}
