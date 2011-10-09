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
import java.io.FileReader;
import java.util.StringTokenizer;

import edu.cmu.sphinx.util.AlignerTestCase;
import edu.cmu.sphinx.util.WordErrorCount;

public class AlignerDemo {
	public static void main(String Args[]) throws Exception {
		BufferedReader batchReader = new BufferedReader(new FileReader(
				"./resource/batchFile.txt"));
		String currFileSet;
		int lineNum = 1;

		while ((currFileSet = batchReader.readLine()) != null) {
			StringTokenizer st = new StringTokenizer(currFileSet);
			if (st.countTokens() != 2) {
				throw new Exception("Error reading batch file at line #"
						+ lineNum);
			}
			String textFile = st.nextToken();
			String audioFile = st.nextToken();

			Aligner aligner = new Aligner("./src/config.xml", audioFile,
					textFile);

			// Simple to use API ;)			
			String reference = aligner.align();
			System.out.println(reference);
			aligner.generateError(1.0f);
			aligner.setOutOfGrammarProbability("1E-3");
			aligner.setPhoneInsertionProbability("1E-60");
			aligner.setRelativeBeamWidth("1E-300");
			aligner.setGrammarType("MODEL_DELETIONS");
			String hypothesis = aligner.align();
			System.out.println(hypothesis);
			WordErrorCount wec = new WordErrorCount(reference, hypothesis);
			wec.align();
		}
	}

}
