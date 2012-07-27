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
	
	public static <T extends Comparable<T>> Fst<T> compose(Fst<T> fst1, Fst<T> fst2, Semiring<T> semiring) {
		if(!fst1.getOsyms().equals(fst2.getIsyms())) {
			// symboltables do not match
			return null;
		}
	
		Fst<T> res = new Fst<T>(semiring);
		
		Mapper<Pair<State<T>, State<T>>, State<T>> stateMap = new Mapper<Pair<State<T>, State<T>>, State<T>>(); 
		ArrayList<Pair<State<T>, State<T>>> queue = new ArrayList<Pair<State<T>, State<T>>>(); 
		
		res.setIsyms(fst1.getIsyms());
		res.setOsyms(fst2.getOsyms());
		
		State<T> s1 = fst1.getStart();
		State<T> s2 = fst2.getStart();
		
		if((s1==null) || (s2==null)) {
			System.out.println("Cannot find initial state.");
			return null;
		}

		Pair<State<T>, State<T>> p = new Pair<State<T>, State<T>>(s1, s2);
		State<T> s = new State<T>(semiring.times(s1.getFinalWeight(), s2.getFinalWeight()));

		res.addState(s);
		res.setStart(s.getId());
		stateMap.put(p, s);
		queue.add(p);
		
		Arc<T> a1;
		Arc<T> a2;
		State<T> nextState1;
		State<T> nextState2;
		Pair<State<T>, State<T>> nextPair;
		State<T> nextState;
		Arc<T> a;
		
		while(queue.size() > 0) {
			p = queue.get(0);
			queue.remove(0);
			s1 = p.getLeft();
			s2 = p.getRight();
			s = stateMap.getValue(p);
			for(Iterator<Arc<T>> itA1 = s1.arcIterator(); itA1.hasNext();) {
				a1 = itA1.next();
				for(Iterator<Arc<T>> itA2 = s2.arcIterator(); itA2.hasNext();) {
					a2 = itA2.next();
					if(a1.getOlabel() == a2.getIlabel()) {
						nextState1 = fst1.getStateById(a1.getNextStateId());
						nextState2 = fst2.getStateById(a2.getNextStateId());
						nextPair = new Pair<State<T>, State<T>>(nextState1, nextState2);
						nextState = stateMap.getValue(nextPair); 
						if(nextState == null) {
							nextState = new State<T>(semiring.times(nextState1.getFinalWeight(), nextState2.getFinalWeight()));
							res.addState(nextState);
							stateMap.put(nextPair, nextState);
							queue.add(nextPair);
						}
						a = new Arc<T>(a1.getIlabel(), a2.getOlabel(), semiring.times(a1.getWeight(), a2.getWeight()), nextState.getId());
						s.addArc(a);
					}
				}
			}
			
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
		
//		Connect.apply(res);

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
		Integer key;
		for(Iterator<Integer> it = syms.keySet().iterator(); it.hasNext();) {
			key = it.next(); 
			newsyms.put(key, syms.getValue(key));
		}
		newsyms.put(newsyms.size(), "<e1>");
		newsyms.put(newsyms.size(), "<e2>");
		
		return newsyms; 
	}
	
	private static <T extends Comparable<T>> Fst<T> augment(int label, Fst<T> fst, Semiring<T> semiring) {
		// label: 0->augment on ilabel
		//        1->augment on olabel

		Fst<T> res = fst;
		Mapper<Integer, String> isyms = res.getIsyms();
		Mapper<Integer, String> osyms = res.getOsyms();

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

		State<T> s;
		Arc<T> arc;
		for(Iterator<State<T>> itS = res.stateIterator(); itS.hasNext();) {
			s = itS.next();
			for(Iterator<Arc<T>> itA = s.arcIterator(); itA.hasNext();) {
				arc = itA.next();
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
