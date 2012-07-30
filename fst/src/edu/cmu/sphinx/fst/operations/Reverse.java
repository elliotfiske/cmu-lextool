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

        for (State is : fst.getStates()) {
            State s = new State(semiring.zero());
            s.setId(is.getId());
            res.addState(s);
            if (is.getFinalWeight() != semiring.zero()) {
                res.setStart(is.getId());
            }
        }

        res.getStateById(fst.getStartId()).setFinalWeight(semiring.one());

        for (State news : res.getStates()) {
            State olds = fst.getStateById(news.getId());
            for (Arc olda : olds.getArcs()) {
                State next = res.getStateById(olda.getNextStateId());
                Arc newa = new Arc(olda.getIlabel(), olda.getOlabel(),
                        semiring.reverse(olda.getWeight()), news.getId());
                next.addArc(newa);
            }
        }

        ExtendFinal.undo(fst);
        return res;
    }
}
