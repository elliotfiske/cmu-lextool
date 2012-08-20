/**
 * 
 */
package edu.cmu.sphinx.sphingid.commons;

/**
 * A class for mathematical functions that are used by other classes.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class MathUtils {
	/**
	 * Finds optimum perplexity threshold by fitting a sixth order polynomial on the data. 
	 * @param perplexities perplexity values for each segment
	 * @param sentencesPerSegment average amount of sentences per segment
	 * @param polyOrder order of the polynomial to be used while calculating optimum
	 * @return the number of sentences that give the lowest perplexity
	 */
	public static int findOptimumThreshold(float[] perplexities,
			int sentencesPerSegment, int polyOrder) {
		float sent[] = new float[perplexities.length];
		float ppNorm[] = new float[perplexities.length];
		for (int i = 0; i < perplexities.length; i++) {
			sent[i] = (i + 1.0F) / perplexities.length;
			ppNorm[i] = perplexities[i] / perplexities[perplexities.length - 1];
		}
		double[] weights = leastSquaresGradientDescent(sent, ppNorm, polyOrder, 0.01);

		double minimum = findMinimum(weights, sent[0], sent[sent.length - 1],
				0.05) * sentencesPerSegment * perplexities.length;

		return (int) minimum;
	}

	/**
	 * Runs least squares using gradient descent to fit a polynomial to a given set of data. Normalization of input data is strongly advised.
	 * @param x
	 * @param y
	 * @param polyOrder order of the polynomial
	 * @param alpha learning rate
	 * @return coefficients of the polynomial as a double array, starting from x^0
	 */
	public static double[] leastSquaresGradientDescent(float[] x, float[] y, int polyOrder,
			double alpha) {
		double theta[] = new double[polyOrder + 1];
		double error[];
		for (int iter = 0; iter < 1000000; iter++) {
			error = new double[polyOrder + 1];
			for (int i = 0; i < x.length; i++) {
				for (int j = 0; j < polyOrder + 1; j++) {
					error[j] += (evalPolyFun(theta, x[i]) - y[i])
							* Math.pow(x[i], j);
				}
			}
			for (int j = 0; j < polyOrder + 1; j++) {
				theta[j] -= alpha * error[j];
			}
		}

		return theta;
	}

	/**
	 * Finds a local minimum in a given interval using golden section method.
	 * @param polyCoefficients coefficients of the polynomial of which local minimum will be found, x^0 is first
	 * @param leftBracket left limit
	 * @param rightBracket right limit 
	 * @param tolerance the algorithm will be run until the difference between left and right brackets are smaller than this value
	 * @return x value that gives the minimum when the polynomial is evaluated
	 */
	public static double findMinimum(double polyCoefficients[],
			double leftBracket, double rightBracket, double tolerance) {
		double rPhi = (Math.sqrt(5.0) - 1) / 2.0;
		double left = leftBracket, right = rightBracket;
		double center1, center2;
		double valCenter1, valCenter2;

		do {
			center1 = left + rPhi*(right - left);
			center2 = right - rPhi*(right - left);
			
			valCenter1 = evalPolyFun(polyCoefficients, center1);
			valCenter2 = evalPolyFun(polyCoefficients, center2);
			
			if (valCenter1 < valCenter2) {
				left = center2;
				center2 = center1;
				center1 = left+ rPhi*(right - left);
			} else
			{
				right = center1;
				center1 = center2;
				center2=right - rPhi*(right - left);
			}
				
			
		} while (right - left > tolerance);

		return (right + left) / 2.0;
	}

	/**
	 * Evaluates a polynomial. 
	 * @param polyCoefficients, x^0 is first
	 * @param x
	 * @return value of the polynomial at x
	 */
	public static double evalPolyFun(double polyCoefficients[], double x) {
		double result = 0;
		
		for (int i = 0; i < polyCoefficients.length; i++) {
			result += polyCoefficients[i] * Math.pow(x, i);
		}

		return result;
	}

}
