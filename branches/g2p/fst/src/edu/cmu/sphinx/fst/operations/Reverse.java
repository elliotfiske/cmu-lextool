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

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Reverse {
	private Reverse() {}
	
	public static <T extends Comparable<T>> Fst<T> get(Fst<T> fst) {
		if(fst.getSemiring() == null) {
			return null;
		}
		
		ExtendFinal.apply(fst);

		Semiring<T> semiring = fst.getSemiring();
		
		Fst<T> res = new Fst<T>(semiring);
		
		res.setIsyms(fst.getOsyms());
		res.setOsyms(fst.getIsyms());
		
		for(int i=0; i < fst.getNumStates(); i++) {
			State<T> is = fst.getStateByIndex(i);
			State<T> s = new State<>(semiring.zero());
			s.setId(is.getId());
			res.addState(s);
			if(!is.getFinalWeight().equals(semiring.zero())) {
				res.setStart(is.getId());
			}
		}
		
		res.getStateById(fst.getStartId()).setFinalWeight(semiring.one());
		
		for(int i=0; i < res.getNumStates(); i++) {
			State<T> news = res.getStateByIndex(i);
			State<T> olds = fst.getStateById(news.getId());
			for(int j=0; j<olds.getNumArcs(); j++) {
				Arc<T> olda = olds.getArc(j);
				State<T> next = res.getStateById(olda.getNextStateId());
				Arc<T> newa = new Arc<T>(olda.getIlabel(), olda.getOlabel(), semiring.reverse(olda.getWeight()),news.getId());
				next.addArc(newa);
			}
			
		}		
		
		ExtendFinal.undo(fst);
		return res;
	}
}
