/**
 * 
 * Copyright 1999-2012 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
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
public class ComposeEpsilonTest {
	@Test
	public void testCompose() {
		System.out.println("Testing Composition with Epsilons...");
		// Input label sort test
	
		Fst<Double> fstA = Convert.importDouble("data/tests/algorithms/composeeps/A", new TropicalSemiring());
		Fst<Double> fstB = Convert.importDouble("data/tests/algorithms/composeeps/B", new TropicalSemiring());
		Fst<Double> fstC = Convert.importDouble("data/tests/algorithms/composeeps/fstcomposeeps", new TropicalSemiring());
		
		Fst<Double> fstComposed = Compose.get(fstA, fstB, new TropicalSemiring());

		assertTrue(fstC.equals(fstComposed));

		System.out.println("Testing Composition with Epsilons Completed!\n");
	}

}
