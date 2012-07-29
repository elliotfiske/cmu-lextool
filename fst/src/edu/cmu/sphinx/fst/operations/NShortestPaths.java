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
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Pair;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class NShortestPaths {
	private NShortestPaths() {}
	
	public static double[] shortestDistance(Fst fst) {
		
		Fst reversed = Reverse.get(fst);
		
		double[] d = new double[reversed.getNumStates()];
		double[] r = new double[reversed.getNumStates()];
		
		Semiring  semiring = reversed.getSemiring();

		for(int i=0;i<d.length; i++) {
			d[i] = semiring.zero();
			r[i] = semiring.zero();
		}
		
		ArrayList<String> queue = new ArrayList<String>();  
		
		queue.add(reversed.getStartId());
		Mapper<Integer, String> ssyms = reversed.getSsyms();
		d[ssyms.getKey(reversed.getStartId())] = semiring.one();
		r[ssyms.getKey(reversed.getStartId())] = semiring.one();
		
		State q;
		double rnew;
		Arc a;
		State nextState;
		double dnext;
		double dnextnew;
		while (queue.size() > 0) {
			q = reversed.getStateById(queue.remove(0));
			rnew = r[ssyms.getKey(q.getId())];
			r[ssyms.getKey(q.getId())] = semiring.zero();
			for(Iterator<Arc> itA = q.arcIterator(); itA.hasNext();) {
				a = itA.next();
				nextState = reversed.getStateById(a.getNextStateId());
				dnext = d[ssyms.getKey(a.getNextStateId())];
				dnextnew = semiring.plus(dnext, semiring.times(rnew, a.getWeight())); 
				if(dnext != dnextnew) {
					d[ssyms.getKey(a.getNextStateId())] = dnextnew;
					r[ssyms.getKey(a.getNextStateId())] = semiring.plus(r[ssyms.getKey(a.getNextStateId())], semiring.times(rnew, a.getWeight()));
					if(!queue.contains(nextState.getId())) {
						queue.add(nextState.getId());
					}
				}
			}
		}

		return d;
	}
	
	
	public static  Fst get(Fst fst, int n, boolean determinize) {
		if( fst == null) {
			return null;
		}
		
		if( fst.getSemiring() == null) {
			return null;
		}
		Fst fstdet = fst;
		if(determinize) {
			fstdet = Determinize.get(fst);
		}
		Semiring semiring = fstdet.getSemiring();
		Fst res = new Fst(semiring);
		res.setIsyms(fstdet.getIsyms());
		res.setOsyms(fstdet.getOsyms());
		
		Mapper<Integer, String> ssyms = fstdet.getSsyms();

		double[] d = shortestDistance(fstdet);		
		
		ExtendFinal.apply(fstdet);

		int[] r = new int[fstdet.getNumStates()];
		for(int i=0;i<r.length; i++) {
			r[i] = 0;
		}
		ArrayList<Pair<State, Double>> queue = new ArrayList<Pair<State, Double>>();
		HashMap<Pair<State, Double>, Pair<State, Double>> previous = new HashMap<Pair<State, Double>, Pair<State, Double>>();
		HashMap<Pair<State, Double>, State> stateMap = new HashMap<Pair<State, Double>, State>();  
		
		State start = fstdet.getStart();
		queue.add(new Pair<State, Double>(start, semiring.one()));
		previous.put(queue.get(0), null);
		
		Pair<State, Double> pair;
		State p;
		double c;
		State s;
		State previouState;
		State previousOldState;
		Arc arc;
		double cnew;
		Pair<State, Double> next;

		while(queue.size() > 0) {
			pair = getLess(queue, d, semiring, ssyms);
			p = pair.getLeft();
			c = pair.getRight();
			
			s = new State(p.getFinalWeight());
			res.addState(s);
			stateMap.put(pair, s);
			if(previous.get(pair) == null) {
				// this is the start state
				res.setStart(s.getId());
			} else {
				// add the incoming arc from previous to current
				previouState = stateMap.get(previous.get(pair));
				previousOldState = previous.get(pair).getLeft();
				for(Iterator<Arc> itA = previousOldState.arcIterator(); itA.hasNext();) {
					arc = itA.next();
					if(arc.getNextStateId().equals(p.getId())) {
						res.addArc(previouState.getId(), new Arc(arc.getIlabel(), arc.getOlabel(), arc.getWeight(), s.getId()));
					}
				}
			}
			
			Integer stateIndex = ssyms.getKey(p.getId());
			r[stateIndex]++;

			if((r[stateIndex] == n) && (p.isFinal())) {
				break;
			}
			
			if(r[stateIndex] <= n) {
				for(Iterator<Arc> itA = p.arcIterator(); itA.hasNext();) {
					arc = itA.next();
					cnew = semiring.times(c, arc.getWeight());
					next = new Pair<State, Double>(fstdet.getStateById(arc.getNextStateId()), cnew); 
					previous.put(next, pair);
					queue.add(next);
				}
			}
		}

		return res;
	}
	
	private static Pair<State, Double> getLess(ArrayList<Pair<State, Double>> queue, double[] d, Semiring semiring, Mapper<Integer, String> ssyms){
		Pair<State, Double> res = queue.get(0);
		
		Pair<State, Double> p;
		State previousState;
		State nextState;
		double previous;
		double next;
		for(Iterator<Pair<State, Double>> it = queue.iterator(); it.hasNext();) {
			p = it.next();
			previousState = res.getLeft();
			nextState = p.getLeft();
			previous = res.getRight();
			next = p.getRight();
			if (semiring.naturalLess(semiring.times(next, d[ssyms.getKey(nextState.getId())]), 
					semiring.times(previous, d[ssyms.getKey(previousState.getId())]))) {
				res = p;
			}
		}
		queue.remove(res);
		
		return res;
	}
}
