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
package edu.cmu.sphinx.diarization;

import java.util.ArrayList;
import java.util.Random;

public class Tester {

	/**
	 * Generates artificial input with distinct speakers based
	 * 
	 * @param vectorSize
	 *            The dimension of a feature vector
	 * @param vectorsCount
	 *            The number of features vectors per speakers
	 * @param speakersCount
	 *            The number of speakers
	 * @return List of features that satisfies the given requirements
	 */
	public static ArrayList<float[]> generateDistinctSpeakers(int vectorSize,
			int vectorsCount, int speakersCount) {
		Random rd = new Random();
		ArrayList<float[]> ret = new ArrayList<float[]>();
		float[] dummy = new float[vectorSize];
		for (int i = 0; i < speakersCount; i++) {
			for (int j = 0; j < vectorSize; j++) {
				dummy[j] = (float) rd.nextInt(5000) / 5000;
			}
			for (int j = 0; j < vectorsCount; j++) {
				float[] copy = new float[vectorSize];
				for (int k = 0; k < vectorSize; k++)
					copy[k] = dummy[k] + (float) rd.nextInt(5000) / 5000;
				ret.add(copy);
			}
		}
		return ret;
	}

	/**
	 * @param speakers
	 *            An array of clusters for which it is needed to be printed the
	 *            speakers intervals
	 */
	public static void printIntervals(ArrayList<SpeakerCluster> speakers) {
		System.out.println("Detected " + speakers.size() + " Speakers :");
		for (int i = 0; i < speakers.size(); i++)
			System.out.println("Speaker " + i + " : "
					+ speakers.get(i).getSpeakerIntervals().toString());
	}

	/**
	 * Test method for SpeakerDiarization, based on artificial input with
	 * non-repeated speakers
	 * 
	 * @param vectorSize
	 *            number of features (Segment.FEATURES_SIZE)
	 * @param vectorsCount
	 *            number of frames for each speaker
	 * @param speakersCount
	 *            number of speakers
	 */
	public static void testDistinctSpeakerDiarization(int vectorSize,
			int vectorsCount, int speakersCount) {
		ArrayList<float[]> ret = generateDistinctSpeakers(vectorSize,
				vectorsCount, speakersCount);
		printIntervals(new SpeakerDiarization().cluster(ret));
	}

	/**
	 * Test method for SpeakerDiarization, based on artificial input with
	 * repeated speakers
	 * 
	 * @param vectorSize
	 *            number of features (Segment.FEATURES_SIZE)
	 * @param vectorsCount
	 *            number of frames for each speaker
	 * @param speakersCount
	 *            number of speakers
	 * @param repeatFactor
	 *            number of times the input should be repeated
	 */
	public static void testRepeatedSpeakerDiarization(int vectorSize,
			int vectorCount, int speakersCount, int repeatFactor) {
		ArrayList<float[]> lst = new ArrayList<float[]>();
		ArrayList<float[]> aux = generateDistinctSpeakers(vectorSize,
				vectorCount, speakersCount);
		for (int i = 0; i < repeatFactor; i++)
			lst.addAll(aux);
		printIntervals(new SpeakerDiarization().cluster(lst));
	}

	/**
	 * Tests SpeakerDiarization on input file given as parameter.
	 * 
	 * @param inputFile
	 *            the input file that needs to be diarized
	 */
	public static void testSpeakerDiarization(String inputFile) {
		printIntervals(new SpeakerDiarization().cluster(inputFile));
	}

	/**
	 * @param args
	 *            -i input file name
	 */
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String inputFile = null;
		for (int i = 0; i < args.length; i++)
			if (args[i].equals("-i"))
				inputFile = args[++i];
		testSpeakerDiarization(inputFile);
		testRepeatedSpeakerDiarization(13, 300, 3, 3);
	}
}
