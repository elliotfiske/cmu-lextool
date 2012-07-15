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

import static org.junit.Assert.*;

import org.junit.Test;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;

public class ArcSortTest {


	/**
	 * Create an output label sorted fst
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ArcSortDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createOsorted() {
		Fst<Double> fst = new Fst<Double>(new TropicalSemiring());
		// State 0
		State<Double> s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(4, 1, 0., "2"));
		s.addArc(new Arc<Double>(5, 2, 0., "2"));
		s.addArc(new Arc<Double>(2, 3, 0., "1"));
		s.addArc(new Arc<Double>(1, 4, 0., "1"));
		s.addArc(new Arc<Double>(3, 5, 0., "1"));

		// State 1
		s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(3, 1, 0., "2"));
		s.addArc(new Arc<Double>(1, 2, 0., "2"));
		s.addArc(new Arc<Double>(2, 3, 0., "1"));

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(0.));
		fst.addState(s);
		
		return fst;
	}
	
	/**
	 * Create an input label sorted fst
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ArcSortDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createIsorted() {
		Fst<Double> fst = new Fst<Double>(new TropicalSemiring());
		// State 0
		State<Double> s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(1, 4, 0., "1"));
		s.addArc(new Arc<Double>(2, 3, 0., "1"));
		s.addArc(new Arc<Double>(3, 5, 0., "1"));
		s.addArc(new Arc<Double>(4, 1, 0., "2"));
		s.addArc(new Arc<Double>(5, 2, 0., "2"));

		// State 1
		s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(1, 2, 0., "2"));
		s.addArc(new Arc<Double>(2, 3, 0., "1"));
		s.addArc(new Arc<Double>(3, 1, 0., "2"));

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(0.));
		fst.addState(s);
		
		return fst;
	}

	/**
	 * Create an unsorted fst
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ArcSortDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createUnsorted() {
		Fst<Double> fst = new Fst<Double>(new TropicalSemiring());
		// State 0
		State<Double> s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(1, 4, 0., "1"));
		s.addArc(new Arc<Double>(3, 5, 0., "1"));
		s.addArc(new Arc<Double>(2, 3, 0., "1"));
		s.addArc(new Arc<Double>(5, 2, 0., "2"));
		s.addArc(new Arc<Double>(4, 1, 0., "2"));

		// State 1
		s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(2, 3, 0., "1"));
		s.addArc(new Arc<Double>(3, 1, 0., "2"));
		s.addArc(new Arc<Double>(1, 2, 0., "2"));

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(0.));
		fst.addState(s);

		return fst;
	}

	@Test
	public void testArcSort() {
		System.out.println("Testing Arc Sort...");
		// Input label sort test
		Fst<Double> fst1 = createUnsorted();
		Fst<Double> fst2 = createIsorted();
		assertTrue(!fst1.equals(fst2));
		ArcSort.apply(fst1, new ILabelCompare<Double>());
		assertTrue(fst1.equals(fst2));
		
		// Output label sort test
		fst1 = createUnsorted();
		fst2 = createOsorted();
		assertTrue(!fst1.equals(fst2));
		ArcSort.apply(fst1, new OLabelCompare<Double>());
		assertTrue(fst1.equals(fst2));

		System.out.println("Testing Arc Sort Completed!\n");

	}

}
