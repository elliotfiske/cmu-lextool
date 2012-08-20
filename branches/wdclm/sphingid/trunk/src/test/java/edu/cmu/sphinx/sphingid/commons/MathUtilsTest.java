/**
 * 
 */
package edu.cmu.sphinx.sphingid.commons;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

/**
 * @author emre
 * 
 */
public class MathUtilsTest {
	/*@SuppressWarnings("static-method")
	@Test
	public void canCalculateGradientDescent() {
		float x[] = { 1.0F, 2.0F, 3.0F, 4.0F };
		float y[] = { 4.0F, 9.0F, 16.0F, 25.0F }; // x^2 + 2x + 1

		double coeffs[] = MathUtils.leastSquaresGradientDescent(x, y, 2, 0.001);

		assertEquals(1.0F, coeffs[0], 0.000005);
		assertEquals(2.0F, coeffs[1], 0.000005);
		assertEquals(1.0F, coeffs[2], 0.000005);
	}*/

	@SuppressWarnings("static-method")
	@Test
	public void canFindOptimumThreshold() {
		float perplexities[] = { 121.2F, 110.27F, 106.1F, 104.25F, 104.16F,
				104.42F, 104.9F, 105.42F, 106.06F, 106.64F, 107.38F, 107.98F,
				109.16F, 110.29F };
		int sentencesPerSegment = 1066000;
		int threshold = MathUtils.findOptimumThreshold(perplexities,
				sentencesPerSegment, 3);
		assertEquals(6000000, threshold, 300000);
	}

	@SuppressWarnings("static-method")
	@Test
	public void canFindMinimum() {
		double polyCoeff[] = { 0.0, 0.0, 1.0 };
		double minimum = MathUtils.findMinimum(polyCoeff, -5, 5, 0.05);
		assertEquals(0, minimum, 0.001);

		double polyCoeff2[] = { 1.0, 1.0, 1.0 };
		minimum = MathUtils.findMinimum(polyCoeff2, -5, 5, 0.001);
		assertEquals(-0.5, minimum, 0.001);
	}

	@SuppressWarnings("static-method")
	@Test
	public void canEvalPoly() {
		double polyCoeff[] = { 1.0, 1.0, 2.0 };
		double result = MathUtils.evalPolyFun(polyCoeff, 2.0);

		assertEquals(11.0, result, 0.01);
	}
}
