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

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Reverse {
    private Reverse() {
    }

    public static Fst get(Fst fst) {
        if (fst.getSemiring() == null) {
            return null;
        }

        ExtendFinal.apply(fst);

        Semiring semiring = fst.getSemiring();

        Fst res = new Fst(semiring);

        res.setIsyms(fst.getOsyms());
        res.setOsyms(fst.getIsyms());

        HashMap<Integer, State> stateMap = new HashMap<Integer, State>();
        for (State is : fst.getStates()) {
            State s = new State(semiring.zero());
            s.setId(is.getId());
            res.addState(s);
            stateMap.put(is.getId(), s);
            if (is.getFinalWeight() != semiring.zero()) {
                res.setStart(s);
            }
        }

        stateMap.get(fst.getStart().getId()).setFinalWeight(semiring.one());

        for (State olds : fst.getStates()) {
            State news = stateMap.get(olds.getId());
            for (Arc olda : olds.getArcs()) {
                State next = stateMap.get(olda.getNextState().getId());
                Arc newa = new Arc(olda.getIlabel(), olda.getOlabel(),
                        semiring.reverse(olda.getWeight()), news);
                next.addArc(newa);
            }
        }

        ExtendFinal.undo(fst);
        return res;
    }
}
