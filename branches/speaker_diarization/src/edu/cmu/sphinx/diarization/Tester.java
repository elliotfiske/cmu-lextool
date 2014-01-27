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

import java.io.FileWriter;
import java.io.IOException;
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
			for (int j = 0; j < vectorSize; j++)
				dummy[j] = (float) (i + 1) / 10 + (float) rd.nextInt(5000)
						/ 50000;
			dummy[0] = 3 + (float) (i + 1) / 10;
			for (int j = 0; j < vectorsCount; j++) {
				float[] copy = new float[vectorSize];
				for (int k = 0; k < vectorSize; k++)
					copy[k] = dummy[k] + (float) rd.nextInt(5000) / 50000;
				ret.add(copy);
			}
		}
		return ret;
	}

	public static String formatedTime(int seconds) {
		return (seconds / 60000) + ":"
				+ (Math.round((double) (seconds % 60000) / 1000));
	}

	/**
	 * @param speakers
	 *            An array of clusters for which it is needed to be printed the
	 *            speakers intervals
	 */
	public static void printIntervals(ArrayList<SpeakerCluster> speakers) {
		System.out.println("Detected " + speakers.size() + " Speakers :");
		for (int i = 0; i < speakers.size(); i++) {
			ArrayList<Integer> t = speakers.get(i).getSpeakerIntervals();
			System.out.print("Speaker " + i + ": ");
			for (int j = 0; j < t.size() / 2; j++)
				System.out.print("[" + formatedTime(t.get(j * 2)) + " "
						+ formatedTime((t.get(j * 2 + 1) + t.get(j * 2)))
						+ "] ");
			System.out.println();
		}
	}

	/**
	 * 
	 * @param speakers
	 *            An array of clusters for which it is needed to be printed the
	 *            speakers intervals
	 * @throws IOException
	 */
	public static void printSpeakerIntervals(
			ArrayList<SpeakerCluster> speakers, String fileName)
			throws IOException {
		String ofName = fileName.substring(0, fileName.indexOf('.'));
		FileWriter fr = new FileWriter(ofName);
		for (int i = 0; i < speakers.size(); i++) {
			ArrayList<Integer> t = speakers.get(i).getSpeakerIntervals();
			for (int j = 0; j < t.size() / 2; j++)
				fr.write(fileName + " " + 1 + " " + t.get(2 * j) / 10 + " "
						+ t.get(2 * j + 1) / 10 + " U U U S" + i + '\n');
		}
		fr.close();
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
	public static void testSpeakerDiarization(String inputFile)
			throws IOException {
		printSpeakerIntervals(new SpeakerDiarization().cluster(inputFile),
				inputFile);
	}

	/**
	 * @param args
	 *            -i input file name
	 */
	public static void main(String[] args) throws IOException {
		String inputFile = null;
		for (int i = 0; i < args.length; i++)
			if (args[i].equals("-i"))
				inputFile = args[++i];
		testSpeakerDiarization(inputFile);
	}
}
