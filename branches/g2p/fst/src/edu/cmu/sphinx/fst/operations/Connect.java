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

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Connect {
    private static void calcCoAccessible(Fst fst, State state,
            ArrayList<ArrayList<State>> paths, ArrayList<State> coaccessible) {
        // hold the coaccessible added in this loop
        ArrayList<State> newCoAccessibles = new ArrayList<State>();
        for (ArrayList<State> path : paths) {
            int index = path.lastIndexOf(state);
            if (index != -1) {
                if (state.getFinalWeight() != fst.getSemiring().zero()
                        || coaccessible.contains(state)) {
                    for (int j = index; j > -1; j--) {
                        if (!coaccessible.contains(path.get(j))) {
                            newCoAccessibles.add(path.get(j));
                            coaccessible.add(path.get(j));
                        }
                    }
                }
            }
        }

        // run again for the new coaccessibles
        for (State s : newCoAccessibles) {
            calcCoAccessible(fst, s, paths, coaccessible);
        }
    }

    private static void duplicatePath(int lastPathIndex, State fromState,
            State toState, ArrayList<ArrayList<State>> paths) {
        ArrayList<State> lastPath = paths.get(lastPathIndex);
        // copy the last path to a new one, from start to current state
        int fromIndex = lastPath.indexOf(fromState);
        int toIndex = lastPath.indexOf(toState);
        if (toIndex == -1) {
            toIndex = lastPath.size() - 1;
        }
        ArrayList<State> newPath = new ArrayList<State>(lastPath.subList(
                fromIndex, toIndex));
        paths.add(newPath);
    }

    private static State dfs(Fst fst, State start,
            ArrayList<ArrayList<State>> paths,
            HashMap<State, ArrayList<Arc>> exploredArcs,
            ArrayList<State> accessible) {
        int lastPathIndex = paths.size() - 1;

        ArrayList<Arc> currentExploredArcs = exploredArcs.get(start);
        paths.get(lastPathIndex).add(start);
        if (start.getNumArcs() != 0) {
            int arcCount = 0;
            for (Arc arc : start.getArcs()) {
                if ((currentExploredArcs == null)
                        || !currentExploredArcs.contains(arc)) {
                    lastPathIndex = paths.size() - 1;
                    if (arcCount++ > 0) {
                        duplicatePath(lastPathIndex, fst.getStart(), start,
                                paths);
                        lastPathIndex = paths.size() - 1;
                        paths.get(lastPathIndex).add(start);
                    }
                    State next = arc.getNextState();
                    addExploredArc(start, arc, exploredArcs, accessible);
                    // detect self loops
                    if (next.getId() != start.getId()) {
                        dfs(fst, next, paths, exploredArcs, accessible);
                    }
                }
            }
        }
        lastPathIndex = paths.size() - 1;
        accessible.add(start);

        return start;
    }

    private static void addExploredArc(State state, Arc arc,
            HashMap<State, ArrayList<Arc>> exploredArcs,
            ArrayList<State> accessible) {
        if (exploredArcs.get(state) == null) {
            exploredArcs.put(state, new ArrayList<Arc>());
        }
        exploredArcs.get(state).add(arc);

    }

    private static void depthFirstSearch(Fst fst, ArrayList<State> accessible,
            ArrayList<ArrayList<State>> paths,
            HashMap<State, ArrayList<Arc>> exploredArcs,
            ArrayList<State> coaccessible) {
        State currentState = fst.getStart();
        State nextState = currentState;
        do {
            if (!accessible.contains(currentState)) {
                nextState = dfs(fst, currentState, paths, exploredArcs,
                        accessible);
            }
        } while (currentState.getId() != nextState.getId());
        for (State s : fst.getStates()) {
            if (s.getFinalWeight() != fst.getSemiring().zero()) {
                calcCoAccessible(fst, s, paths, coaccessible);
            }
        }
    }

    public static void apply(Fst fst) {
        Semiring semiring = fst.getSemiring();
        if (semiring == null) {
            System.out.println("Fst has no semiring.");
            return;
        }

        ArrayList<State> accessible = new ArrayList<State>();
        ArrayList<State> coaccessible = new ArrayList<State>();
        HashMap<State, ArrayList<Arc>> exploredArcs = new HashMap<State, ArrayList<Arc>>();
        ArrayList<ArrayList<State>> paths = new ArrayList<ArrayList<State>>();
        paths.add(new ArrayList<State>());

        depthFirstSearch(fst, accessible, paths, exploredArcs, coaccessible);

        ArrayList<State> toDelete = new ArrayList<State>();

        for (State s : fst.getStates()) {
            if (!(accessible.contains(s) || coaccessible.contains(s))) {
                toDelete.add(s);
            }
        }

        for (State sid : toDelete) {
            fst.deleteState(sid);
        }
    }
}
