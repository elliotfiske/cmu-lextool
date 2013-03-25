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

import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Random;

import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.frontend.DataEndSignal;
import edu.cmu.sphinx.frontend.DoubleData;
import edu.cmu.sphinx.frontend.FloatData;
import edu.cmu.sphinx.frontend.FrontEnd;
import edu.cmu.sphinx.frontend.util.StreamDataSource;
import edu.cmu.sphinx.util.props.ConfigurationManager;

/**
 * Provides method for detecting the number of speakers from a given input file
 * 
 */
public class SpeakerDiarization implements Diarization {

	public final String FRONTEND_NAME = "plpFrontEnd";

	public final String CONFIG_FILE = "frontend.config.xml";

	private FrontEnd frontEnd;
	private StreamDataSource audioSource;
	private ConfigurationManager cm;

	/***
	 * @param inputFile
	 *            the name of the file that needs speaker diarization
	 */
	public SpeakerDiarization(String inputFile) {
		cm = new ConfigurationManager(CONFIG_FILE);
		audioSource = (StreamDataSource) cm.lookup("streamDataSource");
		try {
			audioSource.setInputStream(new FileInputStream(inputFile), "audio");
		} catch (FileNotFoundException e) {
			System.out.println(e.toString());
			System.exit(1);
		}
		frontEnd = (FrontEnd) cm.lookup(FRONTEND_NAME);
	}

	/**
	 * @return The list of feature vectors from the file given as input
	 */
	public ArrayList<float[]> getFeatures() {
		ArrayList<float[]> ret = new ArrayList<float[]>();
		try {
			int featureLength = -1;
			Data feature = frontEnd.getData();
			while (!(feature instanceof DataEndSignal)) {
				if (feature instanceof DoubleData) {
					double[] featureData = ((DoubleData) feature).getValues();
					if (featureLength < 0) {
						featureLength = featureData.length;
					}
					float[] convertedData = new float[featureData.length];
					for (int i = 0; i < featureData.length; i++) {
						convertedData[i] = (float) featureData[i];
					}
					ret.add(convertedData);
				} else if (feature instanceof FloatData) {
					float[] featureData = ((FloatData) feature).getValues();
					if (featureLength < 0) {
						featureLength = featureData.length;

					}
					ret.add(featureData);
				}
				feature = frontEnd.getData();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
		return ret;
	}

	@Override
	/**
	 * @return A cluster for each speaker found in the input file
	 */
	public ArrayList<SpeakerCluster> cluster() {
		ArrayList<SpeakerCluster> ret = new ArrayList<SpeakerCluster>();
		ArrayList<float[]> features = getFeatures();
		Random rd = new Random();
		int speakersCount = rd.nextInt(10);
		int featuresCount = features.size() / speakersCount;
		for (int i = 0; i < speakersCount; i++) {
			SpeakerCluster curent = new SpeakerCluster();
			for (int j = i * featuresCount; j < (i + 1) * featuresCount; j++)
				curent.addSegment(new Segment(j * 10, 10, features.get(j)));
			ret.add(curent);
		}
		return ret;
	}
}
