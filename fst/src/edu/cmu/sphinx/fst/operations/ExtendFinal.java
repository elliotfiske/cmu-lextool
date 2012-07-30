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

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class ExtendFinal {

    private ExtendFinal() {
    }

    public static void apply(Fst fst) {
        Semiring semiring = fst.getSemiring();
        ArrayList<State> fStates = new ArrayList<State>();

        for (State s : fst.getStates()) {
            if (s.getFinalWeight() != semiring.zero()) {
                fStates.add(s);
            }
        }

        // Add a new single final
        State newFinal = new State(semiring.one());
        fst.addState(newFinal);
        for (State s : fStates) {
            // add epsilon transition from the old final to the new one
            s.addArc(new Arc(0, 0, s.getFinalWeight(), newFinal.getId()));
            // set old state's weight to zero
            s.setFinalWeight(semiring.zero());
        }
    }

    public static void undo(Fst fst) {
        // Hopefully, the final state is the last one. We just added it before
        State f = fst.getStateByIndex(fst.getNumStates() - 1);
        // confirm that this is the final
        if (f.getFinalWeight() == fst.getSemiring().zero()) {
            // not a final.
            // TODO: Find it!
        }

        for (State s : fst.getStates()) {
            for (Arc a : s.getArcs()) {
                if (a.getIlabel() == 0 && a.getOlabel() == 0
                        && a.getNextStateId().equals(f.getId())) {
                    s.setFinalWeight(a.getWeight());
                }
            }
        }
        fst.deleteState(f.getId());
    }

}
