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
import java.util.HashSet;
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.weight.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Connect {
	private static <T extends Comparable<T>> void calcCoAccessible(State<T> state, ArrayList<ArrayList<State<T>>> paths, HashSet<State<T>> coaccessible  ) {
		// hold the coaccessible added in this loop
		ArrayList<State<T>> newCoAccessibles = new ArrayList<State<T>>(); 
		for(int i=0;i<paths.size(); i++) {
			ArrayList<State<T>> path = paths.get(i);
			int index = path.lastIndexOf(state);
			if(index != -1) {
				if(state.isFinal() || coaccessible.contains(state)) {
					for(int j=index; j>-1; j--) {
						if(!coaccessible.contains(path.get(j))) {
							newCoAccessibles.add(path.get(j));
							coaccessible.add(path.get(j));
						}
					}
				}
			}
		}
		
		// run again for the new coaccessibles
		for(int i=0;i<newCoAccessibles.size();i++) {
			calcCoAccessible(newCoAccessibles.get(i), paths, coaccessible);
		}
	}

	private static <T extends Comparable<T>> void duplicatePath(int lastPathIndex, State<T> fromState, State<T> toState, ArrayList<ArrayList<State<T>>> paths) {
		ArrayList<State<T>> lastPath = paths.get(lastPathIndex);
		// copy the last path to a new one, from start to current state
		int fromIndex = lastPath.indexOf(fromState);
		int toIndex = lastPath.indexOf(toState);
		if(toIndex == -1) {
			toIndex = lastPath.size() -1;
		}
		ArrayList<State<T>> newPath = new ArrayList<State<T>>(lastPath.subList(fromIndex, toIndex));
		paths.add(newPath);
	}
	
	private static <T extends Comparable<T>> State<T> dfs(Fst<T> fst, State<T> start, ArrayList<ArrayList<State<T>>> paths, Mapper<State<T>, ArrayList<Arc<T>>> exploredArcs , HashSet<State<T>> accessible) {
		int lastPathIndex = paths.size() - 1;

		paths.get(lastPathIndex).add(start);
		if(start.getNumArcs() != 0) {
			int arcCount = 0;
			for(int i=0; i<start.getNumArcs(); i++) {
				Arc<T> arc = start.getArc(i);
				if ((exploredArcs.getValue(start) == null) || !exploredArcs.getValue(start).contains(arc)) {
					lastPathIndex = paths.size() - 1;
					if(arcCount++ > 0) {
						duplicatePath(lastPathIndex, fst.getStart(), start, paths);
						lastPathIndex = paths.size() - 1;
						paths.get(lastPathIndex).add(start);
					}
					State<T> next = fst.getStateById(arc.getNextStateId());
					addExploredArc(start, arc, exploredArcs, accessible);
					// detect self loops
					if(!next.equals(start)) {
						dfs(fst, next, paths, exploredArcs, accessible);
					}
				}
			}
		} 
		lastPathIndex = paths.size() - 1;
		accessible.add(start);
		
		return start;
	}

	private static <T extends Comparable<T>> void addExploredArc(State<T> state, Arc<T> arc, Mapper<State<T>, ArrayList<Arc<T>>> exploredArcs, HashSet<State<T>> accessible) {
		if(!accessible.contains(state)) {
			exploredArcs.put(state, new ArrayList<Arc<T>>());
		}
		exploredArcs.getValue(state).add(arc);
		
	}
	
	private static <T extends Comparable<T>> void depthFirstSearch(Fst<T> fst, HashSet<State<T>> accessible, ArrayList<ArrayList<State<T>>> paths, Mapper<State<T>, ArrayList<Arc<T>>> exploredArcs, HashSet<State<T>> coaccessible ) {
		State<T> currentState = fst.getStart();
		State<T> nextState = currentState;
		do {
			if(!accessible.contains(currentState)) {
				nextState = dfs(fst, currentState, paths, exploredArcs, accessible);
			}
		} while(!currentState.equals(nextState));
		for(int i=0;i<fst.getNumStates();i++) {
			State<T> s = fst.getStateByIndex(i);
			if(s.isFinal()) {
				calcCoAccessible(s, paths, coaccessible);
			}
		}
	}

	public static <T extends Comparable<T>> void apply(Fst<T> fst) {
		Semiring<T> semiring = fst.getSemiring();
		if(semiring == null) {
			System.out.println("Fst has no semiring.");
			return;
		}
		
		HashSet<State<T>> accessible = new HashSet<State<T>>();
		HashSet<State<T>> coaccessible = new HashSet<State<T>>();
		Mapper<State<T>, ArrayList<Arc<T>>> exploredArcs = new Mapper<State<T>, ArrayList<Arc<T>>>();
		ArrayList<ArrayList<State<T>>> paths = new ArrayList<ArrayList<State<T>>>();
		paths.add(new ArrayList<State<T>>());

		depthFirstSearch(fst, accessible, paths, exploredArcs, coaccessible);
		
		ArrayList<String> toDelete = new ArrayList<String>();
		for (int i=0; i<fst.getNumStates(); i++) {
			State<T> s = fst.getStateByIndex(i);
			String id = s.getId(); 
			if(!accessible.contains(s)) { 
				toDelete.add(id);
			} else if (!coaccessible.contains(s)) {
				// inaccessible
				toDelete.add(id);
			}
		}
		
		for(Iterator<String> it = toDelete.iterator(); it.hasNext();) {
			String id = it.next();
			fst.deleteState(id);
		}
	}
}
