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

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ExtendFinal {
	
	private ExtendFinal() {}
	
	public static <T extends Comparable<T>> void apply(Fst<T> fst) {
		Semiring<T> semiring = fst.getSemiring();
		ArrayList<State<T>> fStates = new ArrayList<State<T>>();
		
		State<T> s;
		for(int i=0; i<fst.getNumStates();i++) {
			s = fst.getStateByIndex(i);
			if (!s.getFinalWeight().equals(semiring.zero())) {
				fStates.add(s);
			}
		}
		
		// Add a new single final
		State<T> newFinal = new State<T>(semiring.one());
		fst.addState(newFinal);
		for(int i=0; i<fStates.size(); i++) {
			s = fStates.get(i);
			// add epsilon transition from the old final to the new one 
			s.addArc(new Arc<T>(0, 0, s.getFinalWeight(), newFinal.getId()));
			// set old state's weight to zero
			s.setFinalWeight(semiring.zero());
		}
	}
	
	public static <T extends Comparable<T>> void undo(Fst<T> fst) {
		// Hopefully, the final state is the last one. We just added it before
		State<T> f = fst.getStateByIndex(fst.getNumStates()-1);
		// confirm that this is the final
		if(f.getFinalWeight().equals(fst.getSemiring().zero())) {
			// not a final. 
			// TODO: Find it!
		}
		
		State<T> s;
		Arc<T> a;
		for(int i=0; i<fst.getNumStates(); i++) {
			s = fst.getStateByIndex(i);
			for(int j=0; j<s.getNumArcs(); j++) {
				a = s.getArc(j);
				if(a.getIlabel()==0 && a.getOlabel() == 0 && a.getNextStateId().equals(f.getId())) {
					s.setFinalWeight(a.getWeight());
				}
			}
		}
		fst.deleteState(f.getId());
	}
	


}
