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

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ExtendFinal {
	
	private ExtendFinal() {}
	
	public static void apply(Fst fst) {
		Semiring semiring = fst.getSemiring();
		ArrayList<State> fStates = new ArrayList<State>();
		
		State s;
		for(Iterator<State> itS = fst.stateIterator(); itS.hasNext();) {
			s = itS.next();
			if (s.getFinalWeight() != semiring.zero()) {
				fStates.add(s);
			}
		}
		
		// Add a new single final
		State newFinal = new State(semiring.one());
		fst.addState(newFinal);
		for(Iterator<State> it = fStates.iterator(); it.hasNext();) {
			s = it.next();
			// add epsilon transition from the old final to the new one 
			s.addArc(new Arc(0, 0, s.getFinalWeight(), newFinal.getId()));
			// set old state's weight to zero
			s.setFinalWeight(semiring.zero());
		}
	}
	
	public static void undo(Fst fst) {
		// Hopefully, the final state is the last one. We just added it before
		State f = fst.getStateByIndex(fst.getNumStates()-1);
		// confirm that this is the final
		if(f.getFinalWeight() == fst.getSemiring().zero()) {
			// not a final. 
			// TODO: Find it!
		}
		
		State s;
		Arc a;
		for(Iterator<State> itS = fst.stateIterator(); itS.hasNext();) {
			s = itS.next();
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				a = itA.next();
				if(a.getIlabel()==0 && a.getOlabel() == 0 && a.getNextStateId().equals(f.getId())) {
					s.setFinalWeight(a.getWeight());
				}
			}
		}
		fst.deleteState(f.getId());
	}
	


}
