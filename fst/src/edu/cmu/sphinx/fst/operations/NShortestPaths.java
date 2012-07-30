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
import java.util.HashMap;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Pair;
import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class NShortestPaths {
    private NShortestPaths() {
    }

    public static float[] shortestDistance(Fst fst) {

        Fst reversed = Reverse.get(fst);

        float[] d = new float[reversed.getNumStates()];
        float[] r = new float[reversed.getNumStates()];

        Semiring semiring = reversed.getSemiring();

        for (int i = 0; i < d.length; i++) {
            d[i] = semiring.zero();
            r[i] = semiring.zero();
        }

        ArrayList<String> queue = new ArrayList<String>();

        queue.add(reversed.getStartId());
        Mapper<Integer, String> ssyms = reversed.getSsyms();
        d[ssyms.getKey(reversed.getStartId())] = semiring.one();
        r[ssyms.getKey(reversed.getStartId())] = semiring.one();

        while (queue.size() > 0) {
            State q = reversed.getStateById(queue.remove(0));
            float rnew = r[ssyms.getKey(q.getId())];
            r[ssyms.getKey(q.getId())] = semiring.zero();
            for (Arc a : q.getArcs()) {
                State nextState = reversed.getStateById(a.getNextStateId());
                float dnext = d[ssyms.getKey(a.getNextStateId())];
                float dnextnew = semiring.plus(dnext,
                        semiring.times(rnew, a.getWeight()));
                if (dnext != dnextnew) {
                    d[ssyms.getKey(a.getNextStateId())] = dnextnew;
                    r[ssyms.getKey(a.getNextStateId())] = semiring.plus(
                            r[ssyms.getKey(a.getNextStateId())],
                            semiring.times(rnew, a.getWeight()));
                    if (!queue.contains(nextState.getId())) {
                        queue.add(nextState.getId());
                    }
                }
            }
        }

        return d;
    }

    public static Fst get(Fst fst, int n, boolean determinize) {
        if (fst == null) {
            return null;
        }

        if (fst.getSemiring() == null) {
            return null;
        }
        Fst fstdet = fst;
        if (determinize) {
            fstdet = Determinize.get(fst);
        }
        Semiring semiring = fstdet.getSemiring();
        Fst res = new Fst(semiring);
        res.setIsyms(fstdet.getIsyms());
        res.setOsyms(fstdet.getOsyms());

        Mapper<Integer, String> ssyms = fstdet.getSsyms();

        float[] d = shortestDistance(fstdet);

        ExtendFinal.apply(fstdet);

        int[] r = new int[fstdet.getNumStates()];
        for (int i = 0; i < r.length; i++) {
            r[i] = 0;
        }
        ArrayList<Pair<State, Float>> queue = new ArrayList<Pair<State, Float>>();
        HashMap<Pair<State, Float>, Pair<State, Float>> previous = new HashMap<Pair<State, Float>, Pair<State, Float>>();
        HashMap<Pair<State, Float>, State> stateMap = new HashMap<Pair<State, Float>, State>();

        State start = fstdet.getStart();
        queue.add(new Pair<State, Float>(start, semiring.one()));
        previous.put(queue.get(0), null);

        while (queue.size() > 0) {
            Pair<State, Float> pair = getLess(queue, d, semiring, ssyms);
            State p = pair.getLeft();
            float c = pair.getRight();

            State s = new State(p.getFinalWeight());
            res.addState(s);
            stateMap.put(pair, s);
            if (previous.get(pair) == null) {
                // this is the start state
                res.setStart(s.getId());
            } else {
                // add the incoming arc from previous to current
                State previouState = stateMap.get(previous.get(pair));
                State previousOldState = previous.get(pair).getLeft();
                for (Arc a : previousOldState.getArcs()) {
                    if (a.getNextStateId().equals(p.getId())) {
                        res.addArc(previouState.getId(), new Arc(a.getIlabel(),
                                a.getOlabel(), a.getWeight(), s.getId()));
                    }
                }
            }

            Integer stateIndex = ssyms.getKey(p.getId());
            r[stateIndex]++;

            if ((r[stateIndex] == n) && (p.isFinal())) {
                break;
            }

            if (r[stateIndex] <= n) {
                for (Arc a : p.getArcs()) {
                    float cnew = semiring.times(c, a.getWeight());
                    Pair<State, Float> next = new Pair<State, Float>(
                            fstdet.getStateById(a.getNextStateId()), cnew);
                    previous.put(next, pair);
                    queue.add(next);
                }
            }
        }

        return res;
    }

    private static Pair<State, Float> getLess(
            ArrayList<Pair<State, Float>> queue, float[] d, Semiring semiring,
            Mapper<Integer, String> ssyms) {
        Pair<State, Float> res = queue.get(0);

        for (Pair<State, Float> p : queue) {
            State previousState = res.getLeft();
            State nextState = p.getLeft();
            float previous = res.getRight();
            float next = p.getRight();
            if (semiring.naturalLess(
                    semiring.times(next, d[ssyms.getKey(nextState.getId())]),
                    semiring.times(previous,
                            d[ssyms.getKey(previousState.getId())]))) {
                res = p;
            }
        }
        queue.remove(res);

        return res;
    }
}
