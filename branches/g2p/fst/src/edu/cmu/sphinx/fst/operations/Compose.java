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

import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Pair;
import edu.cmu.sphinx.fst.weight.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */

public class Compose {
	private Compose() {}
	
	private static <T extends Comparable<T>> Fst<T> compose(Fst<T> fst1, Fst<T> fst2, Semiring<T> semiring) {
		Fst<T> res = new Fst<T>(semiring);
		Mapper<Pair<String, String>, Integer> stateMap = new Mapper<Pair<String, String>, Integer>(); 

		if(!fst1.getOsyms().equals(fst2.getIsyms())) {
			// symboltables do not match
			return null;
		}
		
		res.setIsyms(fst1.getIsyms());
		res.setOsyms(fst2.getOsyms());

		// find start state
		int statesCount;
		String start1= fst1.getStartId(); 
		String start2= fst2.getStartId();
		State<T> s1;
		State<T> s2;
		if((start1 != null) && (start2 != null)) {
			s1 = fst1.getStateById(start1);
			s2 = fst2.getStateById(start2);
			stateMap.put(new Pair<String, String>(start1, start2), 0);
			State<T> s = new State<T>(semiring.times(s1.getFinalWeight(), s2.getFinalWeight()));
			String start = res.addState(s);
			statesCount = 1;
			res.setStart(start);
		} else {
			System.out.println("Cannot find initial state.");
			return null;
		}
		int currentState = 0;
		while (currentState < statesCount) {
			Pair<String, String> p = stateMap.getKey(currentState);
			
			s1 = fst1.getStateById(p.getLeft());
			s2 = fst2.getStateById(p.getRight());

			for(int iArc1=0; iArc1<s1.getNumArcs(); iArc1++) {
				Arc<T> arc1 = s1.getArc(iArc1);
				String osym1 = fst1.getOsyms().getValue(arc1.getOlabel());

				for(int iArc2=0; iArc2<s2.getNumArcs(); iArc2++) {
					Arc<T> arc2 = s2.getArc(iArc2);
					String isym2 = fst2.getIsyms().getValue(arc2.getIlabel());
					if(osym1.equals(isym2)) {
						Pair<String, String> p2 = new Pair<String, String>(arc1.getNextStateId(), arc2.getNextStateId());
						Integer newStateIndex = stateMap.getValue(p2); 
						if(newStateIndex == null) {
							State<T> newState = new State<T>(semiring.times(fst1.getStateById(arc1.getNextStateId()).getFinalWeight(), fst2.getStateById(arc2.getNextStateId()).getFinalWeight()));
							res.addState(newState);
							Arc<T> newArc = new Arc<T>(arc1.getIlabel(), 
									arc2.getOlabel(),
									semiring.times(arc1.getWeight(), arc2.getWeight()),
									Integer.toString(statesCount));
							res.addArc(Integer.toString(currentState), newArc);
							stateMap.put(p2, statesCount++);
						} else {
							Arc<T> newArc = new Arc<T>(arc1.getIlabel(), 
									arc2.getOlabel(),
									semiring.times(arc1.getWeight(), arc2.getWeight()),
									Integer.toString(newStateIndex));
							res.addArc(Integer.toString(currentState), newArc);
						}
					}
				}
			}
			currentState++;
		}
		
		return res;
	}
	
	public static <T extends Comparable<T>> Fst<T> get(Fst<T> fst1, Fst<T> fst2, Semiring<T> semiring) {
		if ((fst1 == null) || (fst2 == null)) {
			return null;
		}
		
		if(!fst1.getOsyms().equals(fst2.getIsyms())) {
			// symboltables do not match
			return null;
		}
		
		Fst<T> filter = getFilter(fst1.getOsyms(), semiring);
		Fst<T> afst1 = augment(1, fst1, semiring);
		Fst<T> afst2 = augment(0, fst2, semiring);
		Fst<T> tmp = Compose.compose(afst1, filter, semiring);
		Fst<T> res = Compose.compose(tmp, afst2, semiring); 
		res.getIsyms().removeValue("<e1>");
		res.getIsyms().removeValue("<e2>");
		res.getOsyms().removeValue("<e1>");
		res.getOsyms().removeValue("<e2>");
		
		Connect.apply(res);

		return res;
	}
	
	private static <T extends Comparable<T>> Fst<T> getFilter(Mapper<Integer, String> syms, Semiring<T> semiring) {
		Fst<T> filter = new Fst<T>(semiring);

		Mapper<Integer, String> newSyms = copyAndExtendSyms(syms);
		filter.setIsyms(newSyms);
		filter.setOsyms(newSyms);
		
		// State 0
		State<T> s = new State<T>(semiring.one());
		String start = filter.addState(s);
		s.addArc(new Arc<T>(newSyms.getKey("<e2>"), newSyms.getKey("<e1>"), semiring.one(), "0"));
		s.addArc(new Arc<T>(newSyms.getKey("<e1>"), newSyms.getKey("<e1>"), semiring.one(), "1"));
		s.addArc(new Arc<T>(newSyms.getKey("<e2>"), newSyms.getKey("<e2>"), semiring.one(), "2"));
		for (int i=1;i<syms.size(); i++) {
			s.addArc(new Arc<T>(i, i, semiring.one(), "0"));
		}
		filter.setStart(start);

		// State 1
		s = new State<T>(semiring.one());
		filter.addState(s);
		s.addArc(new Arc<T>(newSyms.getKey("<e1>"), newSyms.getKey("<e1>"), semiring.one(), "1"));
		for (int i=1;i<syms.size(); i++) {
			s.addArc(new Arc<T>(i, i, semiring.one(), "0"));
		}
		
		// State 2
		s = new State<T>(semiring.one());
		filter.addState(s);
		s.addArc(new Arc<T>(newSyms.getKey("<e2>"), newSyms.getKey("<e2>"), semiring.one(), "2"));
		for (int i=1; i<syms.size(); i++) {
			s.addArc(new Arc<T>(i, i, semiring.one(), "0"));
		}

		return filter;
	}
	
	private static Mapper<Integer, String> copyAndExtendSyms(Mapper<Integer, String> syms) {
		Mapper<Integer, String> newsyms = new Mapper<Integer, String>();
		
		for(Iterator<Integer> it = syms.keySet().iterator(); it.hasNext();) {
			Integer key = it.next(); 
			newsyms.put(key, syms.getValue(key));
		}
		newsyms.put(newsyms.size(), "<e1>");
		newsyms.put(newsyms.size(), "<e2>");
		
		return newsyms; 
	}
	
	private static <T extends Comparable<T>> Fst<T> augment(int label, Fst<T> fst, Semiring<T> semiring) {
		// label: 0->augment on ilabel
		//        1->augment on olabel
		Fst<T> res = fst.copy();

		Mapper<Integer, String> isyms = copyAndExtendSyms(fst.getIsyms());
		Mapper<Integer, String> osyms = copyAndExtendSyms(fst.getOsyms());
		
		res.setIsyms(isyms);
		res.setOsyms(osyms);
		
		for(int i=0; i<res.getNumStates(); i++) {
			State<T> s = res.getStateByIndex(i);
			for(int j=0; j<s.getNumArcs();j++) {
				Arc<T> arc = s.getArc(j);
				if((label == 1) && (arc.getOlabel() == 0)) {
					arc.setOlabel(osyms.getKey("<e2>"));
				} else if((label == 0) && (arc.getIlabel() == 0)) {
					arc.setIlabel(isyms.getKey("<e1>"));
				}
			}
			if(label == 0) {
				s.addArc(new Arc<T>(isyms.getKey("<e2>"), 0, semiring.one(), s.getId()));
			} else if(label == 1) {
				s.addArc(new Arc<T>(0, osyms.getKey("<e1>"), semiring.one(), s.getId()));
			}
		}
		return res;
	}
}
