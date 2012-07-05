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

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ComposeEpsilonTest {
	private Fst<Double> createA() {
		TropicalSemiring ts = new TropicalSemiring();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "b");
		isyms.put(3, "c");
		isyms.put(4, "d");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "a");
		osyms.put(2, "d");
		
		Fst<Double> fst = new Fst<Double>();
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 1, 1, 1));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 2, 0, 2));
		fst.addState(s);
		
		// State 2
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 3, 0, 3));
		fst.addState(s);

		// State 3
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 4, 2, 4));
		fst.addState(s);

		// State 4
		s = new State<Double>(ts.one()); 
		fst.addState(s);

		return fst;
	}
	
	private Fst<Double> createB() {
		TropicalSemiring ts = new TropicalSemiring();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "d");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "d");
		osyms.put(2, "e");
		osyms.put(3, "a");
		
		Fst<Double> fst = new Fst<Double>();
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 1, 1, 1));
		fst.addState(s);
		fst.setStart(0);
	
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 0, 2, 2));
		fst.addState(s);
		
		// State 2
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 2, 3, 3));
		fst.addState(s);
		
		// State 3
		s = new State<Double>(ts.one()); 
		fst.addState(s);

		return fst;
	}

	private Fst<Double> createComposed() {
		TropicalSemiring ts = new TropicalSemiring();
		
		SymbolTable isyms = new SymbolTable();
		isyms.put(0, "<eps>");
		isyms.put(1, "a");
		isyms.put(2, "b");
		isyms.put(3, "c");
		isyms.put(4, "d");
		
		SymbolTable osyms = new SymbolTable();
		osyms.put(0, "<eps>");
		osyms.put(1, "d");
		osyms.put(2, "e");
		osyms.put(3, "a");
		
		Fst<Double> fst = new Fst<Double>();
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
		
		// State 0
		State<Double> s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 1, 1, 1));
		fst.addState(s);
		fst.setStart(0);
		
		// State 1
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 2, 2, 2));
		s.AddArc(new Arc<Double>(ts.one(), 2, 0, 3));
		s.AddArc(new Arc<Double>(ts.one(), 0, 2, 4));
		fst.addState(s);

		// State 2
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 3, 0, 5));
		fst.addState(s);

		// State 3
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 3, 0, 6));
		fst.addState(s);

		// State 4
		s = new State<Double>(ts.zero()); 
		fst.addState(s);
		
		// State 5
		s = new State<Double>(ts.zero()); 
		s.AddArc(new Arc<Double>(ts.one(), 4, 3, 7));
		fst.addState(s);
		
		// State 6
		s = new State<Double>(ts.zero()); 
		fst.addState(s);

		// State 7
		s = new State<Double>(ts.one()); 
		fst.addState(s);
		
		return fst;
	}
	
	@Test
	public void testCompose() {
		System.out.println("Testing Composition with Epsilons...");
		// Input label sort test
		Fst<Double> fstA = createA();
		Fst<Double> fstB = createB();
		Fst<Double> fstC = createComposed();
		Fst<Double> fstComposed = Compose.apply(fstA, fstB, new TropicalSemiring());

		 assertTrue(fstC.equals(fstComposed));

		 System.out.println("Testing Composition with Epsilons Completed!\n");
	}

}
