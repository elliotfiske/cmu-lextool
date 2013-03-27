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

	public SpeakerDiarization() {
		cm = new ConfigurationManager(CONFIG_FILE);
		audioSource = (StreamDataSource) cm.lookup("streamDataSource");
		frontEnd = (FrontEnd) cm.lookup(FRONTEND_NAME);
	}

	/**
	 * @return The list of feature vectors from the fileStream used by
	 *         audioSource
	 */
	private ArrayList<float[]> getFeatures() {
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
		double ret = 0, logDet, logDet1, logDet2;
		int nrows = featuresMatrix.getRowDimension(), ncols = featuresMatrix
				.getColumnDimension();
		Array2DRowRealMatrix sub1, sub2;
		sub1 = (Array2DRowRealMatrix) featuresMatrix.getSubMatrix(0, frame - 1,
				0, ncols - 1);
		sub2 = (Array2DRowRealMatrix) featuresMatrix.getSubMatrix(frame,
				nrows - 1, 0, ncols - 1);
		logDet = getLogDet(featuresMatrix);
		logDet1 = getLogDet(sub1);
		logDet2 = getLogDet(sub2);
		ret = nrows * logDet - frame * logDet1 - (nrows - frame) * logDet2;
		return ret;
	}

	/**
	 * @param mat
	 *            A matrix for which is computed log(det(cov(mat)))
	 * @return log(det(cov(mat))) computed as sum of log(eigenvalues)
	 */
	private double getLogDet(Array2DRowRealMatrix mat) {
		double ret = 0;
		EigenDecomposition ed = new EigenDecomposition(
				new Covariance(mat).getCovarianceMatrix());
		double[] re = ed.getRealEigenvalues();
		for (int i = 0; i < re.length; i++)
			ret += Math.log(re[i]);
		return ret;
	}

	@Override
	/**
	 * @param inputFileName The name of the file used for diarization
	 * @return A cluster for each speaker found in the input file
	 */
	public ArrayList<SpeakerCluster> cluster(String inputFileName) {
		try {
			audioSource.setInputStream(new FileInputStream(inputFileName),
					"audio");
		} catch (Exception e) {
			System.out.println(e.toString());
			System.exit(1);
		}
		ArrayList<float[]> features = getFeatures();
		return cluster(features);
	}

	@Override
	/**
	 * @param features The feature vectors to be used for clustering
	 * @return A cluster for each speaker detected based on the feature vectors provided
	 */
	public ArrayList<SpeakerCluster> cluster(ArrayList<float[]> features) {
		ArrayList<SpeakerCluster> ret = new ArrayList<SpeakerCluster>();
		Array2DRowRealMatrix featuresMatrix = ArrayToRealMatrix(features,
				features.size());
		int framesCount = features.size();
		double maxBIC = Double.MIN_VALUE;
		int breakPoint = 0;
		for (int i = Segment.FEATURES_COUNT + 1; i < framesCount
				- Segment.FEATURES_COUNT; i++) {
			double aux = getLikelihoodRatio(i, featuresMatrix);
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
