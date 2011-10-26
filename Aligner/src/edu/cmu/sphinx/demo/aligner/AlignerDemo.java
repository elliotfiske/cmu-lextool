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

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;

import edu.cmu.sphinx.phrasespotter.PhraseSpotter;
import edu.cmu.sphinx.phrasespotter.simplephrasespotter.SimplePhraseSpotter;
import edu.cmu.sphinx.util.AlignerTestCase;
import edu.cmu.sphinx.util.FragmentAudio;
import edu.cmu.sphinx.util.StringCustomise;
import edu.cmu.sphinx.util.WordErrorCount;

public class AlignerDemo {
	public static void main(String Args[]) throws Exception {
		createDB("./resource/batchFile.txt");
		
	}
	
	public static void createDB( String batchFile ) throws Exception {
		BufferedReader batchReader = new BufferedReader(new FileReader(
				batchFile));
		String currFileSet;
		int lineNum = 1;
		
		int fileID = 1;		
		while ((currFileSet = batchReader.readLine()) != null) {
			BufferedWriter writer = new BufferedWriter(new FileWriter(
					"./resource/RitaDB_train.transcription", true));
			BufferedWriter fileIDWriter = new BufferedWriter(new FileWriter(
					"./resource/RitaDB_train.fileids", true));
			StringTokenizer st = new StringTokenizer(currFileSet);
			if (st.countTokens() != 2) {
				throw new Exception("Error reading batch file at line #"
						+ lineNum);
			}
			String textFile = st.nextToken();
			String audioFile = st.nextToken();
			
			Aligner aligner = new Aligner("./src/config.xml", audioFile,
					textFile);
			aligner.setPhraseSpottingConfig("./src/phraseSpotterConfig.xml");
			String reference = aligner.align();
			System.out.println("REFERENCE: " + reference);
			
			// Demonstrating the use of API
			aligner.generateError(0.15f, 0.0f, 0.0f);
			aligner.setGrammarType("MODEL_DELETIONS");
			aligner.setForwardJumpProbability(0.1);
			aligner.performPhraseSpotting(true);
			
			String alignedResult = aligner.align(); // Aligned result
			System.out.println("ALIGNED: " + alignedResult);
			WordErrorCount wec = new WordErrorCount(reference, alignedResult);
			wec.align(); // align and print stats
			
			StringTokenizer tokenizer = new StringTokenizer(alignedResult);
			String wordToken;			
			while (tokenizer.hasMoreTokens()) {
				wordToken = tokenizer.nextToken();
				float startTime = Float.valueOf(getStartTime(wordToken));
				float endTime = Float.valueOf(getEndTime(wordToken));
				String line = getWord(wordToken);
				while ((endTime - startTime) < 12.0f
						&& tokenizer.hasMoreTokens()) {
					wordToken = tokenizer.nextToken();
					endTime = Float.valueOf(getEndTime(wordToken));
					line += " " + getWord(wordToken);
				}
				line = line.toUpperCase();
				line = "<s> " + line + " </s> (" + fileID + ")\n";
				writer.write(line);
				fileIDWriter.write("output/" + fileID + "\n");
				FragmentAudio fa = new FragmentAudio(audioFile);
				String outputFile = "./resource/output/" + fileID + ".wav";
				fa.fragment(outputFile, startTime, endTime);
				fileID ++;
			}
			writer.close();
			fileIDWriter.close();
		}
	}
	public static String getStartTime(String wordToken) {
		String st = wordToken.substring(wordToken.indexOf("(") + 1,
				wordToken.indexOf(","));
		return st;
	}

	public static String getEndTime(String wordToken) {
		String st = wordToken.substring(wordToken.indexOf(",") + 1,
				wordToken.indexOf(")"));
		return st;
	}

	public static String getWord(String wordToken) {
		String st = wordToken.substring(0, wordToken.indexOf("("));
		if(st.compareToIgnoreCase("<unk>") == 0) {
			st =" ";
		}
		return st;
	}

}
