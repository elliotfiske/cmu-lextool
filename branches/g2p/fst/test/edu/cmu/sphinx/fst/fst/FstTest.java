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

package edu.cmu.sphinx.fst.fst;

import static org.junit.Assert.assertTrue;

import org.junit.Test;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.ProbabilitySemiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * Creates the example wfst depicted in Fig. 1 in paper:
 * 
 * C. Allauzen, M. Riley, J. Schalkwyk, W. Skut, M. Mohri, 
 * "OpenFst: a general and efficient weighted finite-state transducer library", 
 * Proceedings of the 12th International Conference on Implementation and 
 * Application of Automata (CIAA 2007), pp. 11–23, Prague, Czech Republic, 
 * July 2007.
 *  
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 *
 */
public class FstTest {

	/**
	 * @param args
	 */
	@Test
	public void testArcMerge() {
		System.out.println("Testing Arc Merging...");
		Fst<Double> fst = new Fst<Double>(new ProbabilitySemiring());
		
		// State 0
		State<Double> s = new State<Double>(0.); 
		fst.addState(s);
		s.addArc(new Arc<Double>(1, 1, 0.5, "1"));
		s.addArc(new Arc<Double>(2, 2, 0.5, "1"));
		s.addArc(new Arc<Double>(2, 2, 1., "1")); 

		assertTrue(s.getNumArcs()==2);
		assertTrue(s.getArc(1).getWeight().getValue().equals(1.5));

		System.out.println("Testing Arc Merging Completed!\n");
	}

	@Test
	public void testStateDeletions() {
		System.out.println("Testing State Deletions...");
		Fst<Double> fst1 = new Fst<Double>(null);
		Fst<Double> fst2 = new Fst<Double>(null);
		
		// State 0
		State<Double> s = new State<Double>(0.); 
		fst1.addState(s);
		s.addArc(new Arc<Double>(0, 0, 0., "1"));
		s.addArc(new Arc<Double>(0, 0, 0., "2"));
		s.addArc(new Arc<Double>(0, 0, 0., "3"));
		fst1.setStart(s.getId());
		
		// State 1
		s = new State<Double>(0.); 
		fst1.addState(s);
		s.addArc(new Arc<Double>(0, 0, 0., "2"));
		
		// State 2
		s = new State<Double>(0.); 
		fst1.addState(s);
		s.addArc(new Arc<Double>(0, 0, 0., "3"));

		// State 3
		s = new State<Double>(0.); 
		fst1.addState(s);
		fst1.setFinal(s.getId(), new Weight<Double>(1.));

		
		// State 0
		s = new State<Double>(0.); 
		s.setId("0");
		fst2.addState(s);
		s.addArc(new Arc<Double>(0, 0, 0., "2"));
		s.addArc(new Arc<Double>(0, 0, 0., "3"));
		fst2.setStart(s.getId());
		
		// State 2
		s = new State<Double>(0.);
		s.setId("2");
		fst2.addState(s);
		s.addArc(new Arc<Double>(0, 0, 0., "3"));
		
		// State 3
		s = new State<Double>(0.);
		s.setId("3");
		fst2.addState(s);
		fst2.setFinal(s.getId(), new Weight<Double>(1.));

		fst1.deleteState("1");
		
		assertTrue(fst1.equals(fst2));

		System.out.println("Testing State Deletions Completed!\n");
	}

}
