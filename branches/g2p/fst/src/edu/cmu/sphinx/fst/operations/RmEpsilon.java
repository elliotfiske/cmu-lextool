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

    private static void put(State fromState, State toState, float weight,
            HashMap<State, HashMap<State, Float>> cl) {
        HashMap<State, Float> tmp = cl.get(fromState);
        if (tmp == null) {
            tmp = new HashMap<State, Float>();
            cl.put(fromState, tmp);
        }
        tmp.put(toState, weight);
    }

    private static void add(State fromState, State toState, float weight,
            HashMap<State, HashMap<State, Float>> cl, Semiring semiring) {
        Float old = getPathWeight(fromState, toState, cl);
        if (old == null) {
            put(fromState, toState, weight, cl);
        } else {
            put(fromState, toState, semiring.plus(weight, old), cl);
        }

    }

    private static void calcClosure(Fst fst, State state,
            HashMap<State, HashMap<State, Float>> cl, Semiring semiring) {
        State s = state;

        float pathWeight;
        for (Arc a : s.getArcs()) {
            if ((a.getIlabel() == 0) && (a.getOlabel() == 0)) {
                if (cl.get(a.getNextState()) == null) {
                    calcClosure(fst, a.getNextState(), cl, semiring);
                }
                if (cl.get(a.getNextState()) != null) {
                    for (State pathFinalState : cl.get(a.getNextState())
                            .keySet()) {
                        pathWeight = semiring.times(
                                getPathWeight(a.getNextState(), pathFinalState,
                                        cl), a.getWeight());
                        add(state, pathFinalState, pathWeight, cl, semiring);
                    }
                }
                add(state, a.getNextState(), a.getWeight(), cl, semiring);
            }
        }
        // Add empty if no outgoing epsilons found
        if (cl.get(state) == null) {
            cl.put(state, null);
        }
    }

    private static Float getPathWeight(State in, State out,
            HashMap<State, HashMap<State, Float>> cl) {
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

        HashMap<State, HashMap<State, Float>> cl = new HashMap<State, HashMap<State, Float>>();
        HashMap<Integer, State> oldToNewStateMap = new HashMap<Integer, State>();
        HashMap<Integer, State> newToOldStateMap = new HashMap<Integer, State>();

        for (State s : fst.getStates()) {
            // Add non-epsilon arcs
            State newState = new State(s.getFinalWeight());
            newState.setId(s.getId());
            res.addState(newState);
            oldToNewStateMap.put(s.getId(), newState);
            newToOldStateMap.put(newState.getId(), s);
            if (newState.getId() == fst.getStart().getId()) {
                res.setStart(newState);
            }
        }

        for (State s : fst.getStates()) {
            // Add non-epsilon arcs
            State newState = oldToNewStateMap.get(s.getId());
            for (Arc a : s.getArcs()) {
                if ((a.getIlabel() != 0) || (a.getOlabel() != 0)) {
                    newState.addArc(new Arc(a.getIlabel(), a.getOlabel(), a
                            .getWeight(), oldToNewStateMap.get(a.getNextState()
                            .getId())));
                }
            }

            // Compute e-Closure
            if (cl.get(s) == null) {
                calcClosure(fst, s, cl, semiring);
            }
        }

        // augment fst with arcs generated from epsilon moves.
        for (State s : res.getStates()) {
            State oldState = newToOldStateMap.get(s.getId());
            if (cl.get(oldState) != null) {
                for (State pathFinalState : cl.get(oldState).keySet()) {
                    State s1 = pathFinalState;
                    if (s1.getFinalWeight() != semiring.zero()) {
                        s.setFinalWeight(semiring.plus(s.getFinalWeight(),
                                semiring.times(getPathWeight(oldState, s1, cl),
                                        s1.getFinalWeight())));
                    }
                    for (Arc a : s1.getArcs()) {
                        if ((a.getIlabel() != 0) || (a.getOlabel() != 0)) {
                            Arc newArc = new Arc(a.getIlabel(), a.getOlabel(),
                                    semiring.times(a.getWeight(),
                                            getPathWeight(oldState, s1, cl)),
                                    oldToNewStateMap.get(a.getNextState()
                                            .getId()));
                            s.addArc(newArc);
                        }
                    }
                }
            }
        }

        res.setIsyms(fst.getIsyms());
        res.setOsyms(fst.getOsyms());

        Connect.apply(res);

        return res;
    }
}
