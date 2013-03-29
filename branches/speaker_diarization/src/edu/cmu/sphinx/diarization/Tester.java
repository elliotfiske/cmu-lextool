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
	 * @param vectorSize
	 *            The dimension of a feature vector
	 * @param vectorsCount
	 *            The number of features vectors per speakers
	 * @param speakersCount
	 *            The number of speakers
	 * @return List of features that satisfies the given requirements
	 */
	static public void testSpeakerDiarization(int vectorSize, int vectorsCount,
			int speakersCount) {
		Random rd = new Random();
		ArrayList<float[]> ret = new ArrayList<float[]>();
		float[] dummy = new float[vectorSize];
		for (int i = 0; i < speakersCount; i++) {
			for (int j = 0; j < vectorSize; j++)
				dummy[j] = (float) rd.nextInt(5000) / 5000;
			for (int j = 0; j < vectorsCount; j++) {
				float[] copy = new float[vectorSize];
				for (int k = 0; k < vectorSize; k++)
					copy[k] = dummy[k] + (float) rd.nextInt(5000) / 500000;
				ret.add(copy);
			}
		}
		SpeakerDiarization sd = new SpeakerDiarization();
		ArrayList<SpeakerCluster> speakers = sd.cluster(ret);
		System.out.println("Detected " + speakers.size() + " Speakers :");
		for (int i = 0; i < speakers.size(); i++)
			System.out.println("Speaker " + i + " : "
					+ speakers.get(i).getSpeakerIntervals().toString());

	}

	static public void testSpeakerDiarization(String inputFile) {
		SpeakerDiarization sd = new SpeakerDiarization();
		ArrayList<SpeakerCluster> speakers = sd.cluster(inputFile);
		System.out.println("Detected " + speakers.size() + " Speakers :");
		for (int i = 0; i < speakers.size(); i++)
			System.out.println("Speaker " + i + " : "
					+ speakers.get(i).getSpeakerIntervals().toString());
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
		testSpeakerDiarization(13, 500, 4);
	}

}
