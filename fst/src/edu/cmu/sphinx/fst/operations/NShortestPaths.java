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
import java.util.HashMap;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Pair;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class NShortestPaths {
	private NShortestPaths() {}
	
	public static<T extends Comparable<T>> Weight<T>[] shortestDistance(Fst<T> fst) {
		
		Fst<T> reversed = Reverse.get(fst);
		
		@SuppressWarnings("unchecked")
		Weight<T>[] d = new Weight[reversed.getNumStates()];
		@SuppressWarnings("unchecked")
		Weight<T>[] r = new Weight[reversed.getNumStates()];
		
		Semiring <T> semiring = reversed.getSemiring();

		for(int i=0;i<d.length; i++) {
			d[i] = new Weight<T>(semiring.zero().getValue());
			r[i] = new Weight<T>(semiring.zero().getValue());
		}
		
		ArrayList<State<T>> queue = new ArrayList<State<T>>();  
		
		queue.add(reversed.getStart());
		Mapper<Integer, String> ssyms = reversed.getSsyms();
		d[ssyms.getKey(reversed.getStartId())] = semiring.one();
		r[ssyms.getKey(reversed.getStartId())] = semiring.one();
		
		while (queue.size() > 0) {
			State<T> q = queue.get(0);
			queue.remove(0);
			Weight<T> rnew = r[ssyms.getKey(q.getId())];
			r[ssyms.getKey(q.getId())] = semiring.zero();
			for(int i=0; i < q.getNumArcs(); i++) {
				Arc<T> a = q.getArc(i);
				State<T> nextState = reversed.getStateById(a.getNextStateId());
				Weight<T> dnext = d[ssyms.getKey(a.getNextStateId())];
				Weight<T> dnextnew = semiring.plus(dnext, semiring.times(rnew, a.getWeight())); 
				if(!dnext.equals(dnextnew)) {
					d[ssyms.getKey(a.getNextStateId())] = dnextnew;
					r[ssyms.getKey(a.getNextStateId())] = semiring.plus(r[ssyms.getKey(a.getNextStateId())], semiring.times(rnew, a.getWeight()));
					if(!queue.contains(nextState)) {
						queue.add(nextState);
					}
				}
			}
		}

		return d;
	}
	
	
	public static <T extends Comparable<T>> Fst<T> get(Fst<T> fst, int n, boolean determinize) {
		if( fst == null) {
			return null;
		}
		
		if( fst.getSemiring() == null) {
			return null;
		}
		Fst<T> fstdet = fst;
		if(determinize) {
			fstdet = Determinize.get(fst);
		}
		Semiring<T> semiring = fstdet.getSemiring();
		Fst<T> res = new Fst<T>(semiring);
		res.setIsyms(fstdet.getIsyms());
		res.setOsyms(fstdet.getOsyms());
		
		Mapper<Integer, String> ssyms = fstdet.getSsyms();

		Weight<T>[] d = shortestDistance(fstdet);		
		
		ExtendFinal.apply(fstdet);

		int[] r = new int[fstdet.getNumStates()];
		for(int i=0;i<r.length; i++) {
			r[i] = 0;
		}
		ArrayList<Pair<State<T>, Weight<T>>> queue = new ArrayList<Pair<State<T>, Weight<T>>>();
		HashMap<Pair<State<T>, Weight<T>>, Pair<State<T>, Weight<T>>> previous = new HashMap<Pair<State<T>, Weight<T>>, Pair<State<T>, Weight<T>>>();
		HashMap<Pair<State<T>, Weight<T>>, State<T>> stateMap = new HashMap<Pair<State<T>, Weight<T>>, State<T>>();  
		
		State<T> start = fstdet.getStart();
		queue.add(new Pair<State<T>, Weight<T>>(start, semiring.one()));
		previous.put(queue.get(0), null);
		
		Pair<State<T>, Weight<T>> pair;
		State<T> p;
		Weight<T> c;
		State<T> s;
		State<T> previouState;
		State<T> previousOldState;
		Arc<T> arc;
		Weight<T> cnew;
		Pair<State<T>, Weight<T>> next;

		while(queue.size() > 0) {
			pair = getLess(queue, d, semiring, ssyms);
			p = pair.getLeft();
			c = pair.getRight();
			
			s = new State<T>(p.getFinalWeight());
			res.addState(s);
			stateMap.put(pair, s);
			if(previous.get(pair) == null) {
				// this is the start state
				res.setStart(s.getId());
			} else {
				// add the incoming arc from previous to current
				previouState = stateMap.get(previous.get(pair));
				previousOldState = previous.get(pair).getLeft();
				for(int i=0; i < previousOldState.getNumArcs(); i++) {
					arc = previousOldState.getArc(i);
					if(arc.getNextStateId().equals(p.getId())) {
						res.addArc(previouState.getId(), new Arc<T>(arc.getIlabel(), arc.getOlabel(), arc.getWeight(), s.getId()));
					}
				}
			}
			
			Integer stateIndex = ssyms.getKey(p.getId());
			r[stateIndex]++;

			if((r[stateIndex] == n) && (p.isFinal())) {
				break;
			}
			
			if(r[stateIndex] <= n) {
				for(int i=0; i<p.getNumArcs(); i++) {
					arc = p.getArc(i);
					cnew = semiring.times(c, arc.getWeight());
					next = new Pair<State<T>, Weight<T>>(fstdet.getStateById(arc.getNextStateId()), cnew); 
					previous.put(next, pair);
					queue.add(next);
				}
			}
		}
		
//		Connect.apply(res);

		return res;
	
	}
	
	private static<T extends Comparable<T>> Pair<State<T>, Weight<T>> getLess(ArrayList<Pair<State<T>, Weight<T>>> queue, Weight<T>[] d, Semiring<T> semiring, Mapper<Integer, String> ssyms){
		Pair<State<T>, Weight<T>> res = queue.get(0);
		int pos = 0;
		
		State<T> previousState;
		State<T> nextState;
		Weight<T> previous;
		Weight<T> next;
		for(int i = 1; i< queue.size(); i++) {
			previousState = res.getLeft();
			nextState = queue.get(i).getLeft();
			previous = res.getRight();
			next = queue.get(i).getRight();
			if (semiring.naturalLess(semiring.times(next, d[ssyms.getKey(nextState.getId())]), 
					semiring.times(previous, d[ssyms.getKey(previousState.getId())]))) {
				pos = i;
				res = queue.get(pos);
			}
		}
		
		queue.remove(pos);
		
		return res;
	}
}
