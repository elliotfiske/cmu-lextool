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
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ComposeTest {

	/**
	 * Create fst A
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ComposeDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createA() {
		TropicalSemiring ts = new TropicalSemiring();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "c");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "q");
		osyms.put(2, "r");
		osyms.put(3, "s");
		
		Fst<Double> fst = new Fst<Double>();
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.), 1, 1, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(2.5), 1, 2, 2));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(0.); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.), 2, 3, 1));
		fst.addState(s);
		
		// State 2
		s = new State<Double>(2.5); 
		fst.addState(s);

		return fst;
	}
	
	/**
	 * Create fst B
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ComposeDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createB() {
		TropicalSemiring ts = new TropicalSemiring();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "q");
		isyms.put(2, "r");
		isyms.put(3, "s");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "f");
		osyms.put(2, "g");
		osyms.put(3, "h");
		osyms.put(4, "j");

		Fst<Double> fst = new Fst<Double>();
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.), 1, 1, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(3.), 2, 3, 2));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(2.5), 3, 2, 2));
		fst.addState(s);
		
		// State 2
		s = new State<Double>(2.); 
		s.AddArc(new Arc<Double>(new Weight<Double>(1.5), 3, 4, 2));
		fst.addState(s);
		
		return fst;
	}

	/**
	 * Create Composed fst 
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ComposeDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createComposed() {
		TropicalSemiring ts = new TropicalSemiring();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "c");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "f");
		osyms.put(2, "g");
		osyms.put(3, "h");
		osyms.put(4, "j");

		Fst<Double> fst = new Fst<Double>();
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(2.), 1, 1, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(5.5), 1, 3, 2));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(new Weight<Double>(3.5), 2, 2, 3));
		fst.addState(s);
		
		// State 2
		s = new State<Double>(4.5); 
		fst.addState(s);
		
		// State 3
		s = new State<Double>(2.); 
		s.AddArc(new Arc<Double>(new Weight<Double>(2.5), 2, 4, 3));
		fst.addState(s);
		
		return fst;
	}
	
	@Test
	public void testCompose() {
		System.out.println("Testing Composition...");
		// Input label sort test
		Fst<Double> fstA = createA();
		Fst<Double> fstB = createB();
		Fst<Double> fstC = createComposed();
		
		Fst<Double> fstComposed = Compose.apply(fstA, fstB, new TropicalSemiring());
		
		assertTrue(fstC.equals(fstComposed));
		
		 System.out.println("Testing Composition Completed!\n");
	}
}
