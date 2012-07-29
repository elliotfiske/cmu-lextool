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

import java.util.ArrayList;
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Pair;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */

public class Compose {
	private Compose() {}
	
	public static Fst compose(Fst fst1, Fst fst2, Semiring semiring) {
		if(!fst1.getOsyms().equals(fst2.getIsyms())) {
			// symboltables do not match
			return null;
		}
	
		Fst res = new Fst(semiring);
		
		Mapper<Pair<State, State>, State> stateMap = new Mapper<Pair<State, State>, State>(); 
		ArrayList<Pair<State, State>> queue = new ArrayList<Pair<State, State>>(); 
		
		res.setIsyms(fst1.getIsyms());
		res.setOsyms(fst2.getOsyms());
		
		State s1 = fst1.getStart();
		State s2 = fst2.getStart();
		
		if((s1==null) || (s2==null)) {
			System.out.println("Cannot find initial state.");
			return null;
		}

		Pair<State, State> p = new Pair<State, State>(s1, s2);
		State s = new State(semiring.times(s1.getFinalWeight(), s2.getFinalWeight()));

		res.addState(s);
		res.setStart(s.getId());
		stateMap.put(p, s);
		queue.add(p);
		
		Arc a1;
		Arc a2;
		State nextState1;
		State nextState2;
		Pair<State, State> nextPair;
		State nextState;
		Arc a;
		while(queue.size() > 0) {
			p = queue.remove(0);
			s1 = p.getLeft();
			s2 = p.getRight();
			s = stateMap.getValue(p);
			for(Iterator<Arc> itA1 = s1.arcIterator(); itA1.hasNext();) {
				a1 = itA1.next();
				for(Iterator<Arc> itA2 = s2.arcIterator(); itA2.hasNext();) {
					a2 = itA2.next();
					if(a1.getOlabel() == a2.getIlabel()) {
						nextState1 = fst1.getStateById(a1.getNextStateId());
						nextState2 = fst2.getStateById(a2.getNextStateId());
						nextPair = new Pair<State, State>(nextState1, nextState2);
						nextState = stateMap.getValue(nextPair); 
						if(nextState == null) {
							nextState = new State(semiring.times(nextState1.getFinalWeight(), nextState2.getFinalWeight()));
							res.addState(nextState);
							stateMap.put(nextPair, nextState);
							queue.add(nextPair);
						}
						a = new Arc(a1.getIlabel(), a2.getOlabel(), semiring.times(a1.getWeight(), a2.getWeight()), nextState.getId());
						s.addArc(a);
					}
				}
			}
			
		}
		
		return res;
	}
	
	public static  Fst get(Fst fst1, Fst fst2, Semiring semiring) {
		if ((fst1 == null) || (fst2 == null)) {
			return null;
		}
		
		if(!fst1.getOsyms().equals(fst2.getIsyms())) {
			// symboltables do not match
			return null;
		}
		
		Fst filter = getFilter(fst1.getOsyms(), semiring);
		augment(1, fst1, semiring);
		augment(0, fst2, semiring);

		Fst tmp = Compose.compose(fst1, filter, semiring);
		
        Fst res = Compose.compose(tmp, fst2, semiring); 
		
//		Connect.apply(res);

		return res;
	}
	
	public static Fst getFilter(Mapper<Integer, String> syms, Semiring semiring) {
		Fst filter = new Fst(semiring);

		if(syms.getKey("<e1>") == null) {
			syms.put(syms.size(), "<e1>");
		}
		if(syms.getKey("<e2>") == null) {
			syms.put(syms.size(), "<e2>");
		}

		
		filter.setIsyms(syms);
		filter.setOsyms(syms);
		
		// State 0
		State s = new State(semiring.one());
		String start = filter.addState(s);
		s.addArc(new Arc(syms.getKey("<e2>"), syms.getKey("<e1>"), semiring.one(), "0"));
		s.addArc(new Arc(syms.getKey("<e1>"), syms.getKey("<e1>"), semiring.one(), "1"));
		s.addArc(new Arc(syms.getKey("<e2>"), syms.getKey("<e2>"), semiring.one(), "2"));
		for(int i=1;i<syms.size() - 2; i++) {
			s.addArc(new Arc(i, i, semiring.one(), "0"));
		}
		filter.setStart(start);

		// State 1
		s = new State(semiring.one());
		filter.addState(s);
		s.addArc(new Arc(syms.getKey("<e1>"), syms.getKey("<e1>"), semiring.one(), "1"));
		for(int i=1;i<syms.size() - 2; i++) {
			s.addArc(new Arc(i, i, semiring.one(), "0"));
		}
		
		// State 2
		s = new State(semiring.one());
		filter.addState(s);
		s.addArc(new Arc(syms.getKey("<e2>"), syms.getKey("<e2>"), semiring.one(), "2"));
		for(int i=1; i<syms.size() - 2; i++) {
			s.addArc(new Arc(i, i, semiring.one(), "0"));
		}

		return filter;
	}
	
	public static void augment(int label, Fst fst, Semiring semiring) {
		// label: 0->augment on ilabel
		//        1->augment on olabel

		Mapper<Integer, String> isyms = fst.getIsyms();
		Mapper<Integer, String> osyms = fst.getOsyms();

		if(isyms.getKey("<e1>") == null) {
			isyms.put(isyms.size(), "<e1>");
		}
		if(isyms.getKey("<e2>") == null) {
			isyms.put(isyms.size(), "<e2>");
		}
		
		if(osyms.getKey("<e1>") == null) {
			osyms.put(osyms.size(), "<e1>");
		}
		if(osyms.getKey("<e2>") == null) {
			osyms.put(osyms.size(), "<e2>");
		}

		State s;
		Arc arc;
		for(Iterator<State> itS = fst.stateIterator(); itS.hasNext();) {
			s = itS.next();
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				arc = itA.next();
				if((label == 1) && (arc.getOlabel() == 0)) {
					arc.setOlabel(osyms.getKey("<e2>"));
				} else if((label == 0) && (arc.getIlabel() == 0)) {
					arc.setIlabel(isyms.getKey("<e1>"));
				}
			}
			if(label == 0) {
				s.addArc(new Arc(isyms.getKey("<e2>"), 0, semiring.one(), s.getId()));
			} else if(label == 1) {
				s.addArc(new Arc(0, osyms.getKey("<e1>"), semiring.one(), s.getId()));
			}
		}
	}
}
