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
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Reverse {
	private Reverse() {}
	
	public static Fst get(Fst fst) {
		if(fst.getSemiring() == null) {
			return null;
		}
		
		ExtendFinal.apply(fst);

		Semiring semiring = fst.getSemiring();
		
		Fst res = new Fst(semiring);
		
		res.setIsyms(fst.getOsyms());
		res.setOsyms(fst.getIsyms());
		
		State is;
		State s;
		for(Iterator<State> itS = fst.stateIterator(); itS.hasNext();) {
			is = itS.next();
			s = new State(semiring.zero());
			s.setId(is.getId());
			res.addState(s);
			if(is.getFinalWeight() != semiring.zero()) {
				res.setStart(is.getId());
			}
		}
		
		res.getStateById(fst.getStartId()).setFinalWeight(semiring.one());
		
		State news;
		State olds;
		Arc olda;
		Arc newa;
		State next;
		for(Iterator<State> itS = res.stateIterator(); itS.hasNext();) {
			news = itS.next();
			olds = fst.getStateById(news.getId());
			for(Iterator<Arc> itA = olds.arcIterator(); itA.hasNext();) {
				olda = itA.next();
				next = res.getStateById(olda.getNextStateId());
				newa = new Arc(olda.getIlabel(), olda.getOlabel(), semiring.reverse(olda.getWeight()),news.getId());
				next.addArc(newa);
			}
		}		
		
		ExtendFinal.undo(fst);
		return res;
	}
}
