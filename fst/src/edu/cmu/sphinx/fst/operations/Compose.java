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
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Pair;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */

public class Compose {
    private Compose() {
    }

    public static Fst compose(Fst fst1, Fst fst2, Semiring semiring) {
        if (!fst1.getOsyms().equals(fst2.getIsyms())) {
            // symboltables do not match
            return null;
        }

        Fst res = new Fst(semiring);

        Mapper<Pair<State, State>, State> stateMap = new Mapper<Pair<State, State>, State>();
        ArrayList<Pair<State, State>> queue = new ArrayList<Pair<State, State>>();

        res.setIsyms(fst1.getIsyms());
        res.setOsyms(fst2.getOsyms());

        State s1 = fst1.getStart();
        State s2 = fst2.getStart();

        if ((s1 == null) || (s2 == null)) {
            System.out.println("Cannot find initial state.");
            return null;
        }

        Pair<State, State> p = new Pair<State, State>(s1, s2);
        State s = new State(semiring.times(s1.getFinalWeight(),
                s2.getFinalWeight()));

        res.addState(s);
        res.setStart(s.getId());
        stateMap.put(p, s);
        queue.add(p);

        while (queue.size() > 0) {
            p = queue.remove(0);
            s1 = p.getLeft();
            s2 = p.getRight();
            s = stateMap.getValue(p);
            for (Arc a1 : s1.getArcs()) {
                for (Arc a2 : s2.getArcs()) {
                    if (a1.getOlabel() == a2.getIlabel()) {
                        State nextState1 = fst1.getStateById(a1
                                .getNextStateId());
                        State nextState2 = fst2.getStateById(a2
                                .getNextStateId());
                        Pair<State, State> nextPair = new Pair<State, State>(
                                nextState1, nextState2);
                        State nextState = stateMap.getValue(nextPair);
                        if (nextState == null) {
                            nextState = new State(semiring.times(
                                    nextState1.getFinalWeight(),
                                    nextState2.getFinalWeight()));
                            res.addState(nextState);
                            stateMap.put(nextPair, nextState);
                            queue.add(nextPair);
                        }
                        Arc a = new Arc(a1.getIlabel(), a2.getOlabel(),
                                semiring.times(a1.getWeight(), a2.getWeight()),
                                nextState.getId());
                        s.addArc(a);
                    }
                }
            }
        }

        return res;
    }

    public static Fst get(Fst fst1, Fst fst2, Semiring semiring) {
        if ((fst1 == null) || (fst2 == null)) {
            return null;
        }

        if (!fst1.getOsyms().equals(fst2.getIsyms())) {
            // symboltables do not match
            return null;
        }

        Fst filter = getFilter(fst1.getOsyms(), semiring);
        augment(1, fst1, semiring);
        augment(0, fst2, semiring);

        Fst tmp = Compose.compose(fst1, filter, semiring);

        Fst res = Compose.compose(tmp, fst2, semiring);

        // Connect.apply(res);

        return res;
    }

    public static Fst getFilter(Mapper<Integer, String> syms, Semiring semiring) {
        Fst filter = new Fst(semiring);

        if (syms.getKey("<e1>") == null) {
            syms.put(syms.size(), "<e1>");
        }
        if (syms.getKey("<e2>") == null) {
            syms.put(syms.size(), "<e2>");
        }

        filter.setIsyms(syms);
        filter.setOsyms(syms);

        // State 0
        State s = new State(semiring.one());
        String start = filter.addState(s);
        s.addArc(new Arc(syms.getKey("<e2>"), syms.getKey("<e1>"), semiring
                .one(), "0"));
        s.addArc(new Arc(syms.getKey("<e1>"), syms.getKey("<e1>"), semiring
                .one(), "1"));
        s.addArc(new Arc(syms.getKey("<e2>"), syms.getKey("<e2>"), semiring
                .one(), "2"));
        for (int i = 1; i < syms.size() - 2; i++) {
            s.addArc(new Arc(i, i, semiring.one(), "0"));
        }
        filter.setStart(start);

        // State 1
        s = new State(semiring.one());
        filter.addState(s);
        s.addArc(new Arc(syms.getKey("<e1>"), syms.getKey("<e1>"), semiring
                .one(), "1"));
        for (int i = 1; i < syms.size() - 2; i++) {
            s.addArc(new Arc(i, i, semiring.one(), "0"));
        }

        // State 2
        s = new State(semiring.one());
        filter.addState(s);
        s.addArc(new Arc(syms.getKey("<e2>"), syms.getKey("<e2>"), semiring
                .one(), "2"));
        for (int i = 1; i < syms.size() - 2; i++) {
            s.addArc(new Arc(i, i, semiring.one(), "0"));
        }

        return filter;
    }

    public static void augment(int label, Fst fst, Semiring semiring) {
        // label: 0->augment on ilabel
        // 1->augment on olabel

        Mapper<Integer, String> isyms = fst.getIsyms();
        Mapper<Integer, String> osyms = fst.getOsyms();

        if (isyms.getKey("<e1>") == null) {
            isyms.put(isyms.size(), "<e1>");
        }
        if (isyms.getKey("<e2>") == null) {
            isyms.put(isyms.size(), "<e2>");
        }

        if (osyms.getKey("<e1>") == null) {
            osyms.put(osyms.size(), "<e1>");
        }
        if (osyms.getKey("<e2>") == null) {
            osyms.put(osyms.size(), "<e2>");
        }

        for (State s : fst.getStates()) {
            for (Arc a : s.getArcs()) {
                if ((label == 1) && (a.getOlabel() == 0)) {
                    a.setOlabel(osyms.getKey("<e2>"));
                } else if ((label == 0) && (a.getIlabel() == 0)) {
                    a.setIlabel(isyms.getKey("<e1>"));
                }
            }
            if (label == 0) {
                s.addArc(new Arc(isyms.getKey("<e2>"), 0, semiring.one(), s
                        .getId()));
            } else if (label == 1) {
                s.addArc(new Arc(0, osyms.getKey("<e1>"), semiring.one(), s
                        .getId()));
            }
        }
    }
}
