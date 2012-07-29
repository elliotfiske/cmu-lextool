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

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.semiring.TropicalSemiring;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ProjectTest {
	/**
	 * Create an fst to Project
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ProjectDoc
	 * 
	 * @return the created fst
	 */
	private Fst createFst() {
		TropicalSemiring ts = new TropicalSemiring();
		Fst fst = new Fst(ts);
		
		Mapper<Integer, String> isyms = new Mapper<Integer, String>();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "b");
		isyms.put(3, "d");
		isyms.put(4, "c");
		isyms.put(5, "f");
		
		Mapper<Integer, String> osyms = new Mapper<Integer, String>();
		osyms.put(0, "<eps>");
		osyms.put(1, "u");
		osyms.put(2, "w");
		osyms.put(3, "v");
		osyms.put(4, "x");
		osyms.put(5, "y");

		// State 0
		State s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(1, 5, 1., "1"));
		s.addArc(new Arc(2, 4, 3., "1"));
		fst.setStart(s.getId());
		
		// State 1
		s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(3, 3, 7., "1"));
		s.addArc(new Arc(4, 2, 5., "2"));

		// State 2
		s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(5, 1, 9., "3"));

		// State 3
		s = new State(2.); 
		fst.addState(s);

		fst.setIsyms(isyms);
		fst.setOsyms(osyms);

		return fst; 
	}
	
	/**
	 * Create the Project on Input Labels
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ProjectDoc
	 * 
	 * @return the created fst
	 */
	private Fst createPi() {
		TropicalSemiring ts = new TropicalSemiring();
		Fst fst = new Fst(ts);
		
		Mapper<Integer, String> syms = new Mapper<Integer, String>();
		syms.put(0, "<eps>");
		syms.put(1, "a");
		syms.put(2, "b");
		syms.put(3, "d");
		syms.put(4, "c");
		syms.put(5, "f");
		
		// State 0
		State s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(1, 1, 1., "1"));
		s.addArc(new Arc(2, 2, 3., "1"));
		fst.setStart(s.getId());
		
		// State 1
		s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(3, 3, 7., "1"));
		s.addArc(new Arc(4, 4, 5., "2"));

		// State 2
		s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(5, 5, 9., "3"));

		// State 3
		s = new State(2.); 
		fst.addState(s);

		fst.setIsyms(syms);
		fst.setOsyms(syms);

		return fst; 
	}

	/**
	 * Create the Project on Output Labels
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ProjectDoc
	 * 
	 * @return the created fst
	 */
	private Fst createPo() {
		TropicalSemiring ts = new TropicalSemiring();
		Fst fst = new Fst(ts);
		
		Mapper<Integer, String> syms = new Mapper<Integer, String>();
		syms.put(0, "<eps>");
		syms.put(1, "u");
		syms.put(2, "w");
		syms.put(3, "v");
		syms.put(4, "x");
		syms.put(5, "y");

		// State 0
		State s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(5, 5, 1., "1"));
		s.addArc(new Arc(4, 4, 3., "1"));
		fst.setStart(s.getId());
		
		// State 1
		s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(3, 3, 7., "1"));
		s.addArc(new Arc(2, 2, 5., "2"));

		// State 2
		s = new State(ts.zero()); 
		fst.addState(s);
		s.addArc(new Arc(1, 1, 9., "3"));

		// State 3
		s = new State(2.); 
		fst.addState(s);

		fst.setIsyms(syms);
		fst.setOsyms(syms);
		return fst; 
	}

	@Test
	public void testProject() {
		System.out.println("Testing Project...");
		// Project on Input label 
		Fst fst = createFst();
		Fst p = createPi();
		Project.apply(fst, ProjectType.INPUT);
		assertTrue(fst.equals(p));
		
		// Project on Output label 
		fst = createFst();
		p = createPo();
		Project.apply(fst, ProjectType.OUTPUT);
		assertTrue(fst.equals(p));

		System.out.println("Testing Project Completed!\n");

	}
	
}
