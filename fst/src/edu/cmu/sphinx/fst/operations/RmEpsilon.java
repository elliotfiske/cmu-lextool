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

import java.util.HashMap;
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class RmEpsilon {
	private RmEpsilon() {}
	

	private static <T extends Comparable<T>>  void put(String fromState, String toState, Weight<T> weight, HashMap<String, HashMap<String, Weight<T>>> cl) {
		HashMap<String, Weight<T>> tmp = cl.get(fromState);
		if(tmp == null) {
			tmp = new  HashMap<String, Weight<T>>();
			cl.put(fromState, tmp);
		}
		tmp.put(toState, weight);
	}
	
	private static <T extends Comparable<T>>  void add(String fromState, String toState, Weight<T> weight, HashMap<String, HashMap<String, Weight<T>>> cl, Semiring <T> semiring) {
		Weight<T> old = getPathWeight(fromState, toState, cl);
		if(old == null) {
			put(fromState, toState, weight, cl);
		} else {
			put(fromState, toState, semiring.plus(weight, old), cl);
		}

	}
	
	private static <T extends Comparable<T>> void calcClosure(Fst<T> fst, String stateid, HashMap<String, HashMap<String, Weight<T>>> cl, Semiring<T> semiring) {
		State<T> s = fst.getStateById(stateid);

		Arc<T> arc;
		String pathFinalState;
		Weight<T> pathWeight;
		for(int i=0; i<s.getNumArcs();i++) {
			arc = s.getArc(i);
			if((arc.getIlabel() == 0) && (arc.getOlabel() == 0)) {
				if (cl.get(arc.getNextStateId()) == null) {
					calcClosure(fst, arc.getNextStateId(), cl, semiring);
				}
				if (cl.get(arc.getNextStateId()) != null) {
					for (Iterator<String> itPath = cl.get(arc.getNextStateId()).keySet().iterator(); itPath.hasNext();) {
						pathFinalState = itPath.next();
						pathWeight = semiring.times(getPathWeight(arc.getNextStateId(), pathFinalState, cl), arc.getWeight());
						add(stateid, pathFinalState, pathWeight, cl, semiring);
					}
				}
				add(stateid, arc.getNextStateId(), arc.getWeight(), cl, semiring);
			}
		}
  		// Add empty if no outgoing epsilons found 
  		if(cl.get(stateid) == null) {
  			cl.put(stateid, null);
  		}
	}
		
	private static <T extends Comparable<T>> Weight<T> getPathWeight(String in, String out, HashMap<String, HashMap<String, Weight<T>>> cl) {
		if (cl.get(in) != null) {
			return cl.get(in).get(out);
		}
		
		return null;
	}

	public static <T extends Comparable<T>> Fst<T> get(Fst<T> fst) {
		if (fst == null) {
			return null;
		}

		if (fst.getSemiring() == null) {
			return null;
		}
		
		HashMap<String, HashMap<String, Weight<T>>> cl;
		Semiring<T> semiring = fst.getSemiring();
		
		Fst<T> res = new Fst<T>(semiring);
		
		cl = new HashMap<String, HashMap<String, Weight<T>>>();
		State<T> s;
		State<T> newState;
		Arc<T> arc;
		for(int i=0; i<fst.getNumStates(); i++) {
			s = fst.getStateByIndex(i);

			// Add non-epsilon arcs
			newState = new State<T>(s.getFinalWeight());
			newState.setId(s.getId());
			res.addState(newState);
			for(int j=0; j<s.getNumArcs(); j++) {
				arc = s.getArc(j);
				if((arc.getIlabel() != 0) || (arc.getOlabel() != 0)) {
					newState.addArc(new Arc<T>(arc.getIlabel(), arc.getOlabel(), arc.getWeight(), arc.getNextStateId()));
				}
			}
			
			// Compute e-Closure
			if(cl.get(s.getId()) == null) {
				calcClosure(fst, s.getId(), cl, semiring);
			}
		}

		// augment fst with arcs generated from epsilon moves.
		String pathFinalState;
		State<T> s1;
		Arc<T> newArc;
		for(int i=0; i<res.getNumStates(); i++) {
			s = res.getStateByIndex(i);
			if(cl.get(s.getId()) != null) {
				for (Iterator<String> it = cl.get(s.getId()).keySet().iterator(); it.hasNext();) {
					pathFinalState = it.next();
					s1 = fst.getStateById(pathFinalState);

					if(!s1.getFinalWeight().equals(semiring.zero())) {
						s.setFinalWeight(semiring.plus(s.getFinalWeight(), semiring.times(getPathWeight(s.getId(), pathFinalState, cl), s1.getFinalWeight())));
					}
					
					for(int j=0; j<s1.getNumArcs(); j++) {
						arc = s1.getArc(j);
						if((arc.getIlabel() != 0) || (arc.getOlabel() != 0)) {
							newArc = new Arc<T>(
									arc.getIlabel(), 
									arc.getOlabel(), 
									semiring.times(arc.getWeight(), getPathWeight(s.getId(), pathFinalState, cl)), 
									arc.getNextStateId()
									);  	
								s.addArc(newArc);
						}					
					}
				}
			}
		}
		
		
		res.setStart(fst.getStartId());
		res.setIsyms(fst.getIsyms());
		res.setOsyms(fst.getOsyms());

		Connect.apply(res);

		return res;
	}
}
