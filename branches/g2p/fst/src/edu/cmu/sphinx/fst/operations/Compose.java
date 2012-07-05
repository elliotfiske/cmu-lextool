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

import com.google.common.collect.HashBiMap;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.fst.SymbolTable;
import edu.cmu.sphinx.fst.openfst.Convert;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Pair;
import edu.cmu.sphinx.fst.weight.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */

public class Compose {
	private Compose() {}
	
	private static <T> Fst<T> compose(Fst<T> fst1, Fst<T> fst2, Semiring<T> semiring) {
		Fst<T> res = new Fst<T>();
		HashBiMap<Pair<Integer, Integer>, Integer> stateMap = HashBiMap.create();

		res.setIsyms(fst1.getIsyms());
		res.setOsyms(fst2.getOsyms());
		
		// find start state
		int statesCount;
		Integer start1= fst1.getStart(); 
		Integer start2= fst2.getStart();
		State<T> s1;
		State<T> s2;
		if((start1 != null) && (start2 != null)) {
			s1 = fst1.getState(start1);
			s2 = fst2.getState(start2);
			stateMap.put(new Pair<Integer, Integer>(start1, start2), 0);
			res.addState(new State<T>(semiring.times(s1.getFinalWeight(), s2.getFinalWeight())));
			statesCount = 1;
			res.setStart(0);
		} else {
			return null;
		}
		int currentState = 0;
		while (currentState < statesCount) {
			Pair<Integer, Integer> p = stateMap.inverse().get(currentState);
			
			s1 = fst1.getState(p.getLeft());
			s2 = fst2.getState(p.getRight());
			for(int i=0; i< s1.getArcs().size(); i++) {
				Arc<T> arc1 = s1.getArc(i);
				String osym1 = fst1.getOsyms().get(arc1.getOlabel());
				for(int j=0; j< s2.getArcs().size(); j++) {
					Arc<T> arc2 = s2.getArc(j);
					String isym2 = fst2.getIsyms().get(arc2.getIlabel());
					if(osym1.equals(isym2)) {
						Pair<Integer, Integer> p2 = new Pair<Integer, Integer>(arc1.getNextState(), arc2.getNextState());
						Integer newStateIndex = stateMap.get(p2); 
						if(newStateIndex == null) {
							State<T> newState = new State<T>(semiring.times(fst1.getState(arc1.getNextState()).getFinalWeight(), fst2.getState(arc2.getNextState()).getFinalWeight()));
							res.addState(newState);
							Arc<T> newArc = new Arc<T>(semiring.times(arc1.getWeight(), arc2.getWeight()), 
									arc1.getIlabel(), 
									arc2.getOlabel(),
									statesCount);
							res.getState(currentState).AddArc(newArc);
							stateMap.put(p2, statesCount++);
						} else {
							Arc<T> newArc = new Arc<T>(semiring.times(arc1.getWeight(), arc2.getWeight()), 
									arc1.getIlabel(), 
									arc2.getOlabel(),
									newStateIndex);
							res.getState(currentState).AddArc(newArc);
						}
					}
					//System.out.println(res);
				}
			}
			currentState++;
		}
		
		return res;
	}
	
	public static <T> Fst<T> apply(Fst<T> fst1, Fst<T> fst2, Semiring<T> semiring) {
		if ((fst1 == null) || (fst2 == null)) {
			return null;
		}
		
		if(!fst1.getOsyms().equals(fst2.getIsyms())) {
			// symboltables do not match
			return null;
		}
		
		Fst<T> filter = getFilter(fst1.getOsyms(), semiring);
		Convert.export(filter, semiring, "/home/john/projects/cmusphinx/software/data/20120705/Filter");
		Fst<T> afst1 = augment(1, fst1, filter.getOsyms(), semiring);
		Convert.export(afst1, semiring, "/home/john/projects/cmusphinx/software/data/20120705/AT");
		Fst<T> afst2 = augment(0, fst2, filter.getIsyms(), semiring);
		Convert.export(afst2, semiring, "/home/john/projects/cmusphinx/software/data/20120705/BT");
		
		return Compose.compose(Compose.compose(afst1, filter, semiring), afst2, semiring);
	}
	
	private static <T> Fst<T> getFilter(SymbolTable syms, Semiring<T> semiring) {
		Fst<T> filter = new Fst<T>();
		
		// extend symbol table
		SymbolTable newSyms = new SymbolTable();
		for (int i=0; i<syms.size(); i++) {
			newSyms.put(i, syms.get(i));
		}
		int e1 = newSyms.size();
		newSyms.put(newSyms.size(), "e1");
		int e2 = newSyms.size();
		newSyms.put(newSyms.size(), "e2");

		filter.setIsyms(newSyms);
		filter.setOsyms(newSyms);
		
		// State 0
		State<T> s = new State<T>(semiring.one());
		s.AddArc(new Arc<T>(semiring.one(), e2, e1, 0));
		s.AddArc(new Arc<T>(semiring.one(), e1, e1, 1));
		s.AddArc(new Arc<T>(semiring.one(), e2, e2, 2));
		for (int i=1;i<syms.size(); i++) {
			s.AddArc(new Arc<T>(semiring.one(), i, i, 0));
		}
		filter.addState(s);
		filter.setStart(0);

		// State 1
		s = new State<T>(semiring.one());
		s.AddArc(new Arc<T>(semiring.one(), e1, e1, 1));
		for (int i=1;i<syms.size(); i++) {
			s.AddArc(new Arc<T>(semiring.one(), i, i, 0));
		}
		filter.addState(s);
		
		// State 2
		s = new State<T>(semiring.one());
		s.AddArc(new Arc<T>(semiring.one(), e2, e2, 2));
		for (int i=1; i<syms.size(); i++) {
			s.AddArc(new Arc<T>(semiring.one(), i, i, 0));
		}
		filter.addState(s);

		return filter;
	}
	
	private static <T> Fst<T> augment(int label, Fst<T> fst, SymbolTable syms, Semiring<T> semiring) {
		// label: 0->augment on ilabel
		//        1->augment on olabel
		Fst<T> res = fst.copy();
		int e1 = syms.size() - 2;
		int e2 = syms.size() - 1;
		
		if(label==0) {
			res.setIsyms(syms);
		} else if(label==1) {
			res.setOsyms(syms);
		}
		

		for(int i=0; i<res.getNumStates(); i++) {
			State<T> s = res.getState(i);
			for(int j=0; j< s.getNumArcs(); j++) {
				Arc<T> arc = s.getArc(j);
				if((label == 1) && (arc.getOlabel() == 0)) {
					arc.setOlabel(e2);
				} else if((label == 0) && (arc.getIlabel() == 0)) {
					arc.setIlabel(e1);
				}
			}
			if(label == 0) {
				s.AddArc(new Arc<T>(semiring.one(), e2, 0, i));
			} else if(label == 1) {
				s.AddArc(new Arc<T>(semiring.one(), 0, e1, i));
			}
		}
		
		return res;
		
	}
}
