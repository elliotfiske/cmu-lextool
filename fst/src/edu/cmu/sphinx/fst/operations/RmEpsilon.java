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
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class RmEpsilon {
	private RmEpsilon() {}
	

	private static  void put(String fromState, String toState, double weight, HashMap<String, HashMap<String, Double>> cl) {
		HashMap<String, Double> tmp = cl.get(fromState);
		if(tmp == null) {
			tmp = new  HashMap<String, Double>();
			cl.put(fromState, tmp);
		}
		tmp.put(toState, weight);
	}
	
	private static  void add(String fromState, String toState, double weight, HashMap<String, HashMap<String, Double>> cl, Semiring  semiring) {
		Double old = getPathWeight(fromState, toState, cl);
		if(old == null) {
			put(fromState, toState, weight, cl);
		} else {
			put(fromState, toState, semiring.plus(weight, old), cl);
		}

	}
	
	private static void calcClosure(Fst fst, String stateid, HashMap<String, HashMap<String, Double>> cl, Semiring semiring) {
		State s = fst.getStateById(stateid);

		Arc arc;
		String pathFinalState;
		double pathWeight;
		for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
			arc = itA.next();
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
		
	private static Double getPathWeight(String in, String out, HashMap<String, HashMap<String, Double>> cl) {
		if (cl.get(in) != null) {
			return cl.get(in).get(out);
		}
		
		return null;
	}

	public static Fst get(Fst fst) {
		if (fst == null) {
			return null;
		}

		if (fst.getSemiring() == null) {
			return null;
		}
		
		HashMap<String, HashMap<String, Double>> cl;
		Semiring semiring = fst.getSemiring();
		
		Fst res = new Fst(semiring);
		
		cl = new HashMap<String, HashMap<String, Double>>();
		State s;
		State newState;
		Arc arc;
		for(Iterator<State> itS = fst.stateIterator(); itS.hasNext();) {
			s = itS.next();
			// Add non-epsilon arcs
			newState = new State(s.getFinalWeight());
			newState.setId(s.getId());
			res.addState(newState);
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				arc = itA.next();
				if((arc.getIlabel() != 0) || (arc.getOlabel() != 0)) {
					newState.addArc(new Arc(arc.getIlabel(), arc.getOlabel(), arc.getWeight(), arc.getNextStateId()));
				}
			}
			
			// Compute e-Closure
			if(cl.get(s.getId()) == null) {
				calcClosure(fst, s.getId(), cl, semiring);
			}
		}

		// augment fst with arcs generated from epsilon moves.
		String pathFinalState;
		State s1;
		Arc newArc;
		for(Iterator<State> itS = res.stateIterator(); itS.hasNext();) {
			s = itS.next();
			if(cl.get(s.getId()) != null) {
				for (Iterator<String> it = cl.get(s.getId()).keySet().iterator(); it.hasNext();) {
					pathFinalState = it.next();
					s1 = fst.getStateById(pathFinalState);

					if(s1.getFinalWeight() != semiring.zero()) {
						s.setFinalWeight(semiring.plus(s.getFinalWeight(), semiring.times(getPathWeight(s.getId(), pathFinalState, cl), s1.getFinalWeight())));
					}
					for(Iterator<Arc> itA = s1.arcIterator(); itA.hasNext();) {
						arc = itA.next();
						if((arc.getIlabel() != 0) || (arc.getOlabel() != 0)) {
							newArc = new Arc(
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
