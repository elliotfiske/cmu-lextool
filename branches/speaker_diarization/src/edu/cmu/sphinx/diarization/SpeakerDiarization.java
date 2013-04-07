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
import java.text.DecimalFormat;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedList;

import org.apache.commons.math3.linear.Array2DRowRealMatrix;
import org.apache.commons.math3.linear.EigenDecomposition;
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
	private double getLikelihoodRatio(int frame, Array2DRowRealMatrix features) {
		double logDet, logDet1, logDet2;
		int d = Segment.FEATURES_SIZE;
		double penalty = 0.5 * (d + 0.5 * d * (d + 1))
				* Math.log(features.getRowDimension()) * 4;
		int nrows = features.getRowDimension(), ncols = features
				.getColumnDimension();
		Array2DRowRealMatrix sub1, sub2;
		sub1 = (Array2DRowRealMatrix) features.getSubMatrix(0, frame - 1, 0,
				ncols - 1);
		sub2 = (Array2DRowRealMatrix) features.getSubMatrix(frame, nrows - 1,
				0, ncols - 1);
		logDet = getLogDet(features);
		logDet1 = getLogDet(sub1);
		logDet2 = getLogDet(sub2);
		return (nrows * logDet - frame * logDet1 - (nrows - frame) * logDet2 - penalty);
	}

	/**
	 * @param start
	 *            The starting frame
	 * @param length
	 *            The length of the interval, as numbers of frames
	 * @param features
	 *            The matrix build with feature vectors as rows
	 * @return Returns the changing point in the input represented by features
	 * 
	 */

	private int getPoint(int start, int length, int step,
			Array2DRowRealMatrix features) {
		double max = Double.NEGATIVE_INFINITY;
		int ncols = features.getColumnDimension(), point = 0;
		Array2DRowRealMatrix sub = (Array2DRowRealMatrix) features
				.getSubMatrix(start, start + length - 1, 0, ncols - 1);
		for (int i = Segment.FEATURES_SIZE + 1; i < length
				- Segment.FEATURES_SIZE; i += step) {
			double aux = getLikelihoodRatio(i, sub);
			if (aux > max) {
				max = aux;
				point = i;
			}
		}
		if (max < 0)
			point = Integer.MIN_VALUE;
		return point + start;
	}

	/**
	 * @param features
	 *            Matrix with feature vectors as rows
	 * @return A list with all changing points detected in the file
	 */
	private LinkedList<Integer> getAllChangingPoints(
			Array2DRowRealMatrix features) {
		LinkedList<Integer> ret = new LinkedList<Integer>();
		ret.add(0);
		int framesCount = features.getRowDimension(), step = 100;
		int start = 0, end = step, cp;
		while (end < framesCount) {
			cp = getPoint(start, end - start + 1, step / 10, features);
			if (cp > 0) {
				if (cp - ret.get(ret.size() - 1) > step / 2) {
					start = cp;
					end = start + step;
					ret.add(cp);
				} else
					end += step;
			} else
				end += step;
		}
		ret.add(framesCount);
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
		LinkedList<Integer> l = getAllChangingPoints(featuresMatrix);
		Iterator<Integer> it = l.iterator();
		int curent, previous = it.next();
		while (it.hasNext()) {
			curent = it.next();
			SpeakerCluster c = new SpeakerCluster(new Segment(previous
					* Segment.FRAME_LENGTH, (curent - previous)
					* Segment.FRAME_LENGTH),
					(Array2DRowRealMatrix) featuresMatrix.getSubMatrix(
							previous, curent - 1, 0, 12));
			ret.add(c);
			previous = curent;
		}
		int clusterCount = ret.size();
		Array2DRowRealMatrix distance;
		distance = new Array2DRowRealMatrix(clusterCount, clusterCount);
		distance = updateDistances(ret);
		while (true) {
			double distmin = 0;
			int imin = -1, jmin = -1;
			for (int i = 0; i < clusterCount; i++) {
				for (int j = 0; j < clusterCount; j++) {
					if (distance.getEntry(i, j) < distmin
							&& distance.getEntry(i, j) <= 0 && i != j) {
						distmin = distance.getEntry(i, j);
						imin = i;
						jmin = j;
					}
				}
			}
			if (imin == -1) {
				break;
			}
			ret.get(imin).mergeWith(ret.get(jmin));
			updateDistances(ret, imin, jmin);
			ret.remove(jmin);
			clusterCount--;
		}
		return ret;
	}

	/**
	 * @param Clustering
	 *            The array of clusters
	 * @param posi
	 *            The index of the merged cluster
	 * @param posj
	 *            The index of the cluster that will be eliminated from the
	 *            clustering
	 */
	Array2DRowRealMatrix updateDistances(ArrayList<SpeakerCluster> clustering,
			int posi, int posj) {
		int clusterCount = clustering.size();
		Array2DRowRealMatrix distance = new Array2DRowRealMatrix(clusterCount,
				clusterCount);
		for (int i = 0; i < clusterCount; i++) {
			distance.setEntry(i, posi,
					computeDistance(clustering.get(i), clustering.get(posi)));
			distance.setEntry(posi, i,
					computeDistance(clustering.get(i), clustering.get(posi)));
		}
		for (int i = posj; i < clusterCount - 1; i++) {
			for (int j = 0; j < clusterCount; j++) {
				distance.setEntry(i, j, distance.getEntry(i + 1, j));
				distance.setEntry(j, i, distance.getEntry(j, i + 1));
			}
		}
		return (Array2DRowRealMatrix) distance.getSubMatrix(0,
				clusterCount - 1, 0, clusterCount - 1);
	}

	/**
	 * @param Clustering
	 *            The array of clusters
	 */
	Array2DRowRealMatrix updateDistances(ArrayList<SpeakerCluster> clustering) {
		int clusterCount = clustering.size();
		Array2DRowRealMatrix distance = new Array2DRowRealMatrix(clusterCount,
				clusterCount);
		for (int i = 0; i < clusterCount; i++) {
			for (int j = 0; j <= i; j++) {
				distance.setEntry(i, j,
						computeDistance(clustering.get(i), clustering.get(j)));
				distance.setEntry(j, i,
						computeDistance(clustering.get(i), clustering.get(j)));
			}
		}
		return distance;
	}

	double computeDistance(SpeakerCluster c1, SpeakerCluster c2) {
		int rowDim = c1.getFeatureMatrix().getRowDimension()
				+ c2.getFeatureMatrix().getRowDimension();
		int colDim = c1.getFeatureMatrix().getColumnDimension();
		Array2DRowRealMatrix combinedFeatures = new Array2DRowRealMatrix(
				rowDim, colDim);
		combinedFeatures.setSubMatrix(c1.getFeatureMatrix().getData(), 0, 0);
		combinedFeatures.setSubMatrix(c2.getFeatureMatrix().getData(), c1
				.getFeatureMatrix().getRowDimension(), 0);
		return getLikelihoodRatio(c1.getFeatureMatrix().getRowDimension(),
				combinedFeatures);
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
