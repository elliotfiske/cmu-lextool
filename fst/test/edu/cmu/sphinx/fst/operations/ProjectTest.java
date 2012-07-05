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
import edu.cmu.sphinx.fst.fst.SymbolTable;
import edu.cmu.sphinx.fst.openfst.Convert;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;

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
	private Fst<Double> createFst() {
		TropicalSemiring ts = new TropicalSemiring();
		Fst<Double> fst = new Fst<>();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "b");
		isyms.put(3, "d");
		isyms.put(4, "c");
		isyms.put(5, "f");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "u");
		osyms.put(2, "w");
		osyms.put(3, "v");
		osyms.put(4, "x");
		osyms.put(5, "y");

		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.), 1, 5, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(3.), 2, 4, 1));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(7.), 3, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(5.), 4, 2, 2));
		fst.addState(s);

		// State 2
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(9.), 5, 1, 3));
		fst.addState(s);

		// State 3
		s = new State<Double>(new Weight<Double>(2.)); 
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
	private Fst<Double> createPi() {
		TropicalSemiring ts = new TropicalSemiring();
		Fst<Double> fst = new Fst<>();
		
		SymbolTable syms = new SymbolTable();
		syms.put(0, "<eps>");
		syms.put(1, "a");
		syms.put(2, "b");
		syms.put(3, "d");
		syms.put(4, "c");
		syms.put(5, "f");
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.), 1, 1, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(3.), 2, 2, 1));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(7.), 3, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(5.), 4, 4, 2));
		fst.addState(s);

		// State 2
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(9.), 5, 5, 3));
		fst.addState(s);

		// State 3
		s = new State<Double>(new Weight<Double>(2.)); 
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
	private Fst<Double> createPo() {
		TropicalSemiring ts = new TropicalSemiring();
		Fst<Double> fst = new Fst<>();
		
		SymbolTable syms = new SymbolTable();
		syms.put(0, "<eps>");
		syms.put(1, "u");
		syms.put(2, "w");
		syms.put(3, "v");
		syms.put(4, "x");
		syms.put(5, "y");

		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.), 5, 5, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(3.), 4, 4, 1));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(7.), 3, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(5.), 2, 2, 2));
		fst.addState(s);

		// State 2
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(9.), 1, 1, 3));
		fst.addState(s);

		// State 3
		s = new State<Double>(new Weight<Double>(2.)); 
		fst.addState(s);

		fst.setIsyms(syms);
		fst.setOsyms(syms);
		return fst; 
	}

	@Test
	public void testProject() {
		System.out.println("Testing Project...");
		// Project on Input label 
		Fst<Double> fst = createFst();
		Convert.export(fst, new TropicalSemiring(), "/home/john/projects/cmusphinx/software/data/20120705/A");
		Fst<Double> p = createPi();
		Convert.export(p, new TropicalSemiring(), "/home/john/projects/cmusphinx/software/data/20120705/Pi");
		Project.apply(fst, ProjectType.INPUT);
		Convert.export(fst, new TropicalSemiring(), "/home/john/projects/cmusphinx/software/data/20120705/Project_i");
		assertTrue(fst.equals(p));
		
		// Project on Output label 
		fst = createFst();
		p = createPo();
		Convert.export(p, new TropicalSemiring(), "/home/john/projects/cmusphinx/software/data/20120705/Po");
		Project.apply(fst, ProjectType.OUTPUT);
		Convert.export(fst, new TropicalSemiring(), "/home/john/projects/cmusphinx/software/data/20120705/Project_o");
		assertTrue(fst.equals(p));

		System.out.println("Testing Project Completed!\n");

	}
	
}
