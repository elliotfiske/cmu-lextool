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
    private static void calcCoAccessible(Fst fst, Integer stateId,
            ArrayList<ArrayList<Integer>> paths, ArrayList<Integer> coaccessible) {
        // hold the coaccessible added in this loop
        ArrayList<Integer> newCoAccessibles = new ArrayList<Integer>();
        for (ArrayList<Integer> path : paths) {
            int index = path.lastIndexOf(stateId);
            if (index != -1) {
                if (fst.getStateById(stateId).getFinalWeight() != fst
                        .getSemiring().zero() || coaccessible.contains(stateId)) {
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
        for (Integer s : newCoAccessibles) {
            calcCoAccessible(fst, s, paths, coaccessible);
        }
    }

    private static void duplicatePath(int lastPathIndex, State fromState,
            State toState, ArrayList<ArrayList<Integer>> paths) {
        ArrayList<Integer> lastPath = paths.get(lastPathIndex);
        // copy the last path to a new one, from start to current state
        int fromIndex = lastPath.indexOf(fromState.getId());
        int toIndex = lastPath.indexOf(toState.getId());
        if (toIndex == -1) {
            toIndex = lastPath.size() - 1;
        }
        ArrayList<Integer> newPath = new ArrayList<Integer>(lastPath.subList(
                fromIndex, toIndex));
        paths.add(newPath);
    }

    private static State dfs(Fst fst, State start,
            ArrayList<ArrayList<Integer>> paths,
            HashMap<Integer, ArrayList<Arc>> exploredArcs,
            ArrayList<Integer> accessible) {
        int lastPathIndex = paths.size() - 1;

        ArrayList<Arc> currentExploredArcs = exploredArcs.get(start.getId());
        paths.get(lastPathIndex).add(start.getId());
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
                        paths.get(lastPathIndex).add(start.getId());
                    }
                    State next = fst.getStateById(arc.getNextStateId());
                    addExploredArc(start, arc, exploredArcs, accessible);
                    // detect self loops
                    if (!next.equals(start)) {
                        dfs(fst, next, paths, exploredArcs, accessible);
                    }
                }
            }
        }
        lastPathIndex = paths.size() - 1;
        accessible.add(start.getId());

        return start;
    }

    private static void addExploredArc(State state, Arc arc,
            HashMap<Integer, ArrayList<Arc>> exploredArcs,
            ArrayList<Integer> accessible) {
        if (!accessible.contains(state.getId())) {
            exploredArcs.put(state.getId(), new ArrayList<Arc>());
        }
        exploredArcs.get(state.getId()).add(arc);

    }

    private static void depthFirstSearch(Fst fst,
            ArrayList<Integer> accessible, ArrayList<ArrayList<Integer>> paths,
            HashMap<Integer, ArrayList<Arc>> exploredArcs,
            ArrayList<Integer> coaccessible) {
        State currentState = fst.getStart();
        State nextState = currentState;
        do {
            if (!accessible.contains(currentState.getId())) {
                nextState = dfs(fst, currentState, paths, exploredArcs,
                        accessible);
            }
        } while (!currentState.equals(nextState));
        for (State s : fst.getStates()) {
            if (s.getFinalWeight() != fst.getSemiring().zero()) {
                calcCoAccessible(fst, s.getId(), paths, coaccessible);
            }
        }
    }

    public static void apply(Fst fst) {
        Semiring semiring = fst.getSemiring();
        if (semiring == null) {
            System.out.println("Fst has no semiring.");
            return;
        }

        ArrayList<Integer> accessible = new ArrayList<Integer>();
        ArrayList<Integer> coaccessible = new ArrayList<Integer>();
        HashMap<Integer, ArrayList<Arc>> exploredArcs = new HashMap<Integer, ArrayList<Arc>>();
        ArrayList<ArrayList<Integer>> paths = new ArrayList<ArrayList<Integer>>();
        paths.add(new ArrayList<Integer>());

        depthFirstSearch(fst, accessible, paths, exploredArcs, coaccessible);

        ArrayList<Integer> toDelete = new ArrayList<Integer>();

        for (State s : fst.getStates()) {
            Integer id = s.getId();
            if (!(accessible.contains(id) || coaccessible.contains(id))) {
                toDelete.add(id);
            }
        }

        for (Integer sid : toDelete) {
            fst.deleteState(sid);
        }
    }
}
