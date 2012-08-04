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
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.utils.Pair;

/**
 * 
 * New
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */

public class Compose {
    private Compose() {
    }

    public static Fst compose(Fst fst1, Fst fst2, Semiring semiring,
            boolean sorted) {
        if (!fst1.getOsyms().equals(fst2.getIsyms())) {
            // symboltables do not match
            return null;
        }

        Fst res = new Fst(semiring);

        HashMap<Pair<State, State>, State> stateMap = new HashMap<Pair<State, State>, State>();
        ArrayList<Pair<State, State>> queue = new ArrayList<Pair<State, State>>();

        State s1 = fst1.getStart();
        State s2 = fst2.getStart();

        if ((s1 == null) || (s2 == null)) {
            System.err.println("Cannot find initial state.");
            return null;
        }

        Pair<State, State> p = new Pair<State, State>(s1, s2);
        State s = new State(semiring.times(s1.getFinalWeight(),
                s2.getFinalWeight()));

        res.addState(s);
        res.setStart(s);
        stateMap.put(p, s);
        queue.add(p);

        while (queue.size() > 0) {
            p = queue.remove(0);
            s1 = p.getLeft();
            s2 = p.getRight();
            s = stateMap.get(p);
            for (Arc a1 : s1.getArcs()) {
                for (Arc a2 : s2.getArcs()) {
                    if (sorted && a1.getOlabel() < a2.getIlabel())
                        break;
                    if (a1.getOlabel() == a2.getIlabel()) {
                        State nextState1 = a1.getNextState();
                        State nextState2 = a2.getNextState();
                        Pair<State, State> nextPair = new Pair<State, State>(
                                nextState1, nextState2);
                        State nextState = stateMap.get(nextPair);
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
                                nextState);
                        s.addArc(a);
                    }
                }
            }
        }

        res.setIsyms(fst1.getIsyms());
        res.setOsyms(fst2.getOsyms());

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

        Fst tmp = Compose.compose(fst1, filter, semiring, false);

        Fst res = Compose.compose(tmp, fst2, semiring, false);

        // Connect.apply(res);

        return res;
    }

    public static Fst getFilter(HashMap<String, Integer> syms, Semiring semiring) {
        Fst filter = new Fst(semiring);

        Integer e1index = syms.get("<e1>");
        if (e1index == null) {
            e1index = syms.size();
            syms.put("<e1>", syms.size());
        }
        Integer e2index = syms.get("<e2>");
        if (e2index == null) {
            e2index = syms.size();
            syms.put("<e2>", syms.size());
        }

        filter.setIsyms(syms);
        filter.setOsyms(syms);

        // State 0
        State s0 = new State(semiring.one());
        State s1 = new State(semiring.one());
        State s2 = new State(semiring.one());
        filter.addState(s0);
        s0.addArc(new Arc(e2index, e1index, semiring.one(), s0));
        s0.addArc(new Arc(e1index, e1index, semiring.one(), s1));
        s0.addArc(new Arc(e2index, syms.get("<e2>"), semiring.one(), s2));
        for (int i = 1; i < syms.size() - 2; i++) {
            s0.addArc(new Arc(i, i, semiring.one(), s0));
        }
        filter.setStart(s0);

        // State 1
        filter.addState(s1);
        s1.addArc(new Arc(e1index, e1index, semiring.one(), s1));
        for (int i = 1; i < syms.size() - 2; i++) {
            s1.addArc(new Arc(i, i, semiring.one(), s0));
        }

        // State 2
        filter.addState(s2);
        s2.addArc(new Arc(e2index, e2index, semiring.one(), s2));
        for (int i = 1; i < syms.size() - 2; i++) {
            s2.addArc(new Arc(i, i, semiring.one(), s0));
        }

        return filter;
    }

    public static void augment(int label, Fst fst, Semiring semiring) {
        // label: 0->augment on ilabel
        // 1->augment on olabel

        HashMap<String, Integer> isyms = fst.getIsyms();
        HashMap<String, Integer> osyms = fst.getOsyms();

        Integer e1inputIndex = isyms.get("<e1>");
        if (e1inputIndex == null) {
            e1inputIndex = isyms.size();
            isyms.put("<e1>", isyms.size());
        }
        Integer e2inputIndex = isyms.get("<e2>");
        if (e2inputIndex == null) {
            e2inputIndex = isyms.size();
            isyms.put("<e2>", isyms.size());
        }

        Integer e1outputIndex = osyms.get("<e1>");
        if (e1outputIndex == null) {
            e1outputIndex = osyms.size();
            osyms.put("<e1>", osyms.size());
        }
        Integer e2outputIndex = osyms.get("<e2>");
        if (e2outputIndex == null) {
            e2outputIndex = osyms.size();
            osyms.put("<e2>", osyms.size());
        }

        for (State s : fst.getStates()) {
            for (Arc a : s.getArcs()) {
                if ((label == 1) && (a.getOlabel() == 0)) {
                    a.setOlabel(e2outputIndex);
                } else if ((label == 0) && (a.getIlabel() == 0)) {
                    a.setIlabel(e1inputIndex);
                }
            }
            if (label == 0) {
                s.addArc(new Arc(e2inputIndex, 0, semiring.one(), s));
            } else if (label == 1) {
                s.addArc(new Arc(0, e1outputIndex, semiring.one(), s));
            }
        }
    }
}
