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

public class Tester {

	static public void testSpeakerDiarization(String inputFile) {
		SpeakerDiarization sd = new SpeakerDiarization(inputFile);
		ArrayList<SpeakerCluster> speakers = sd.cluster();
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
		testSpeakerDiarization(inputFile);
	}

}
