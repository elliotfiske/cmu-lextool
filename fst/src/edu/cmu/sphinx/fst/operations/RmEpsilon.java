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
public class RmEpsilon {
    private RmEpsilon() {
    }

    private static void put(Integer fromState, Integer toState, float weight,
            HashMap<Integer, HashMap<Integer, Float>> cl) {
        HashMap<Integer, Float> tmp = cl.get(fromState);
        if (tmp == null) {
            tmp = new HashMap<Integer, Float>();
            cl.put(fromState, tmp);
        }
        tmp.put(toState, weight);
    }

    private static void add(Integer fromState, Integer toState, float weight,
            HashMap<Integer, HashMap<Integer, Float>> cl, Semiring semiring) {
        Float old = getPathWeight(fromState, toState, cl);
        if (old == null) {
            put(fromState, toState, weight, cl);
        } else {
            put(fromState, toState, semiring.plus(weight, old), cl);
        }

    }

    private static void calcClosure(Fst fst, Integer stateid,
            HashMap<Integer, HashMap<Integer, Float>> cl, Semiring semiring) {
        State s = fst.getStateById(stateid);

        float pathWeight;
        for (Arc a : s.getArcs()) {
            if ((a.getIlabel() == 0) && (a.getOlabel() == 0)) {
                if (cl.get(a.getNextStateId()) == null) {
                    calcClosure(fst, a.getNextStateId(), cl, semiring);
                }
                if (cl.get(a.getNextStateId()) != null) {
                    for (Integer pathFinalState : cl.get(a.getNextStateId())
                            .keySet()) {
                        pathWeight = semiring.times(
                                getPathWeight(a.getNextStateId(),
                                        pathFinalState, cl), a.getWeight());
                        add(stateid, pathFinalState, pathWeight, cl, semiring);
                    }
                }
                add(stateid, a.getNextStateId(), a.getWeight(), cl, semiring);
            }
        }
        // Add empty if no outgoing epsilons found
        if (cl.get(stateid) == null) {
            cl.put(stateid, null);
        }
    }

    private static Float getPathWeight(Integer in, Integer out,
            HashMap<Integer, HashMap<Integer, Float>> cl) {
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

        Semiring semiring = fst.getSemiring();

        Fst res = new Fst(semiring);

        HashMap<Integer, HashMap<Integer, Float>> cl = new HashMap<Integer, HashMap<Integer, Float>>();

        for (State s : fst.getStates()) {
            // Add non-epsilon arcs
            State newState = new State(s.getFinalWeight());
            newState.setId(s.getId());
            res.addState(newState);
            for (Arc a : s.getArcs()) {
                if ((a.getIlabel() != 0) || (a.getOlabel() != 0)) {
                    newState.addArc(new Arc(a.getIlabel(), a.getOlabel(), a
                            .getWeight(), a.getNextStateId()));
                }
            }

            // Compute e-Closure
            if (cl.get(s.getId()) == null) {
                calcClosure(fst, s.getId(), cl, semiring);
            }
        }

        // augment fst with arcs generated from epsilon moves.
        for (State s : res.getStates()) {
            if (cl.get(s.getId()) != null) {
                for (Integer pathFinalState : cl.get(s.getId()).keySet()) {
                    State s1 = fst.getStateById(pathFinalState);

                    if (s1.getFinalWeight() != semiring.zero()) {
                        s.setFinalWeight(semiring.plus(s.getFinalWeight(),
                                semiring.times(
                                        getPathWeight(s.getId(),
                                                pathFinalState, cl), s1
                                                .getFinalWeight())));
                    }
                    for (Arc a : s1.getArcs()) {
                        if ((a.getIlabel() != 0) || (a.getOlabel() != 0)) {
                            Arc newArc = new Arc(a.getIlabel(), a.getOlabel(),
                                    semiring.times(
                                            a.getWeight(),
                                            getPathWeight(s.getId(),
                                                    pathFinalState, cl)),
                                    a.getNextStateId());
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
