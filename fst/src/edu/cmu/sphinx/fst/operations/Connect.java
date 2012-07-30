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

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Connect {
    private static void calcCoAccessible(Fst fst, String stateId,
            ArrayList<ArrayList<String>> paths, ArrayList<String> coaccessible) {
        // hold the coaccessible added in this loop
        ArrayList<String> newCoAccessibles = new ArrayList<String>();
        for (ArrayList<String> path : paths) {
            int index = path.lastIndexOf(stateId);
            if (index != -1) {
                if (fst.getStateById(stateId).isFinal()
                        || coaccessible.contains(stateId)) {
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
        for (String s : newCoAccessibles) {
            calcCoAccessible(fst, s, paths, coaccessible);
        }
    }

    private static void duplicatePath(int lastPathIndex, State fromState,
            State toState, ArrayList<ArrayList<String>> paths) {
        ArrayList<String> lastPath = paths.get(lastPathIndex);
        // copy the last path to a new one, from start to current state
        int fromIndex = lastPath.indexOf(fromState.getId());
        int toIndex = lastPath.indexOf(toState.getId());
        if (toIndex == -1) {
            toIndex = lastPath.size() - 1;
        }
        ArrayList<String> newPath = new ArrayList<String>(lastPath.subList(
                fromIndex, toIndex));
        paths.add(newPath);
    }

    private static State dfs(Fst fst, State start,
            ArrayList<ArrayList<String>> paths,
            Mapper<String, ArrayList<Arc>> exploredArcs,
            ArrayList<String> accessible) {
        int lastPathIndex = paths.size() - 1;

        paths.get(lastPathIndex).add(start.getId());
        if (start.getNumArcs() != 0) {
            int arcCount = 0;
            for (Arc arc : start.getArcs()) {
                if ((exploredArcs.getValue(start.getId()) == null)
                        || !exploredArcs.getValue(start.getId()).contains(arc)) {
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
            Mapper<String, ArrayList<Arc>> exploredArcs,
            ArrayList<String> accessible) {
        if (!accessible.contains(state.getId())) {
            exploredArcs.put(state.getId(), new ArrayList<Arc>());
        }
        exploredArcs.getValue(state.getId()).add(arc);

    }

    private static void depthFirstSearch(Fst fst, ArrayList<String> accessible,
            ArrayList<ArrayList<String>> paths,
            Mapper<String, ArrayList<Arc>> exploredArcs,
            ArrayList<String> coaccessible) {
        State currentState = fst.getStart();
        State nextState = currentState;
        do {
            if (!accessible.contains(currentState.getId())) {
                nextState = dfs(fst, currentState, paths, exploredArcs,
                        accessible);
            }
        } while (!currentState.equals(nextState));
        for (State s : fst.getStates()) {
            if (s.isFinal()) {
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

        ArrayList<String> accessible = new ArrayList<String>();
        ArrayList<String> coaccessible = new ArrayList<String>();
        Mapper<String, ArrayList<Arc>> exploredArcs = new Mapper<String, ArrayList<Arc>>();
        ArrayList<ArrayList<String>> paths = new ArrayList<ArrayList<String>>();
        paths.add(new ArrayList<String>());

        depthFirstSearch(fst, accessible, paths, exploredArcs, coaccessible);

        ArrayList<String> toDelete = new ArrayList<String>();

        for (State s : fst.getStates()) {
            String id = s.getId();
            if (!accessible.contains(id)) {
                toDelete.add(id);
            } else if (!coaccessible.contains(id)) {
                // inaccessible
                toDelete.add(id);
            }
        }

        for (String sid : toDelete) {
            fst.deleteState(sid);
        }
    }
}
