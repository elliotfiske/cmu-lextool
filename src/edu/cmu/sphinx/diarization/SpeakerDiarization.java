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
import java.text.DecimalFormat;
import java.util.ArrayList;

import org.apache.commons.math3.linear.Array2DRowRealMatrix;
import org.apache.commons.math3.linear.BlockRealMatrix;
import org.apache.commons.math3.linear.EigenDecomposition;
import org.apache.commons.math3.linear.RealMatrix;
import org.apache.commons.math3.stat.correlation.Covariance;

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

	/**
	 * The function considers there are only 2 speakers
	 * 
	 * @param frame
	 *            First speaker has frames [0, frame] and the other one [frame,
	 *            ...]
	 * @return Maximum likelihood ratio
	 */
	private double getLikelihoodRatio(int frame,
			Array2DRowRealMatrix featuresMatrix) {
		double ret = 0, det, det1, det2;
		int nrows = featuresMatrix.getRowDimension(), ncols = featuresMatrix
				.getColumnDimension();
		Array2DRowRealMatrix sub1, sub2;
		sub1 = (Array2DRowRealMatrix) featuresMatrix.getSubMatrix(0, frame - 1,
				0, ncols - 1);
		sub2 = (Array2DRowRealMatrix) featuresMatrix.getSubMatrix(frame,
				nrows - 1, 0, ncols - 1);
		det = new EigenDecomposition(
				new Covariance(featuresMatrix).getCovarianceMatrix())
				.getDeterminant();
		det1 = new EigenDecomposition(
				new Covariance(sub1).getCovarianceMatrix()).getDeterminant();
		det2 = new EigenDecomposition(
				new Covariance(sub2).getCovarianceMatrix()).getDeterminant();
		ret = nrows * Math.log(det) - frame * Math.log(det1) - (nrows - frame)
				* Math.log(det2);
		if (det <= 0 || det1 <= 0 || det2 <= 0)
			System.out.println(det + " " + det1 + " " + det2 + " " + frame);
		return ret;
	}

	@Override
	/**
	 * @return A cluster for each speaker found in the input file
	 */
	public ArrayList<SpeakerCluster> cluster() {
		ArrayList<SpeakerCluster> ret = new ArrayList<SpeakerCluster>();
		ArrayList<float[]> features = Tester.generateFeatures(13, 100, 2);
		Array2DRowRealMatrix featuresMatrix = ArrayToRealMatrix(features,
				features.size());
		int framesCount = features.size();
		double maxBIC = Double.MIN_VALUE;
		System.out.println(maxBIC);
		int breakPoint = 0;
		for (int i = Segment.FEATURES_COUNT + 1; i < framesCount
				- Segment.FEATURES_COUNT; i++) {
			double aux = getLikelihoodRatio(i, featuresMatrix);
			// System.out.println(aux);
			if (aux > maxBIC) {
				breakPoint = i;
				maxBIC = aux;
			}
		}
		ret.add(new SpeakerCluster());
		ret.add(new SpeakerCluster());
		for (int i = 0; i < framesCount; i++)
			ret.get((i >= breakPoint ? 1 : 0)).addSegment(
					new Segment(i * 10, 10, features.get(i)));
		return ret;
	}

	/**
	 * @param lst
	 *            An ArrayList with all the values being vectors of the same
	 *            dimension
	 * @return The RealMatrix with the vectors from the ArrayList on columns
	 */
	Array2DRowRealMatrix ArrayToRealMatrix(ArrayList<float[]> lst, int size) {
		int length = lst.get(1).length;
		Array2DRowRealMatrix ret = new Array2DRowRealMatrix(size, length);
		int i = 0;
		for (i = 0; i < size; i++) {
			double[] converted = new double[length];
			for (int j = 0; j < length; j++)
				converted[j] = (double) ((lst.get(i))[j]);
			ret.setRow(i, converted);
		}
		return ret;
	}

	void printMatrix(Array2DRowRealMatrix a) {
		DecimalFormat df = new DecimalFormat("#.######");
		for (int i = 0; i < a.getRowDimension(); i++) {
			for (int j = 0; j < a.getColumnDimension(); j++)
				System.out.print(a.getEntry(i, j) + " ");
			System.out.println();
		}
	}
}
