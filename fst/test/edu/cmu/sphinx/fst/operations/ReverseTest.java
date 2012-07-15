/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import static org.junit.Assert.assertTrue;

import org.junit.Test;

import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.openfst.Convert;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ReverseTest {
	@Test
	public void testReverse() {
		System.out.println("Testing Reverse...");
		// Input label sort test
	
		Fst<Double> fst = Convert.importDouble("data/tests/algorithms/reverse/A", new TropicalSemiring());
		Fst<Double> fstB = Convert.importDouble("data/tests/algorithms/reverse/fstreverse", new TropicalSemiring());
		
		Fst<Double> fstReversed = Reverse.get(fst);

		assertTrue(fstB.equals(fstReversed));

		System.out.println("Testing Reverse Completed!\n");
	}

}
