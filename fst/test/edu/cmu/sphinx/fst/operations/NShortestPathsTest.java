/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import static org.junit.Assert.*;

import org.junit.Test;

import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.openfst.Convert;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class NShortestPathsTest {

	@Test
	public void testNShortestPaths() {
		System.out.println("Testing NShortestPaths...");
		
		Fst<Double> fst = Convert.importDouble("data/tests/algorithms/shortestpath/A", new TropicalSemiring());
		Fst<Double> nsp = Convert.importDouble("data/tests/algorithms/shortestpath/nsp", new TropicalSemiring());
		
		Fst<Double> fstNsp = NShortestPaths.get(fst, 6, true);
		
		assertTrue(nsp.equals(fstNsp));
		
		System.out.println("Testing NShortestPaths Completed!\n");
	}
}
