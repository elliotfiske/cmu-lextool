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


package edu.cmu.sphinx.fst.fst;

import java.io.Serializable;
import java.util.ArrayList;

import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <W>
 */
public class Fst<W extends Weight<?>> implements Serializable {
	
	private static final long serialVersionUID = -7821842965064006073L;
	
	// holds the fst states
	private ArrayList<State<W>> states = new ArrayList<State<W>>();
	
	// the initial state
	private int start;

	/**
	 * 
	 */
	public Fst() {	}

	/**
	 * @return the initial state
	 */
	public int getStart() {
		return start;
	}

	/**
	 * @param start the initial state to set
	 */
	public void setStart(int start) {
		this.start = start;
	}
	
	/**
	 * Return the weight of a final state
	 * 
	 * @param sIndex the state
	 * @return the state's weight
	 */
	public W getFinal(int sIndex) {
		return this.states.get(sIndex).getFinalWeight();
	}
	
	/**
	 * Sets the weight of a final state
	 * 
	 * @param sIndex the state
	 * @param w the state's weight
	 */
	public void setFinal(int sIndex, W w) {
		this.states.get(sIndex).setFinalWeight(w);
	}

	/**
	 * 
	 * @return the number of states in the fst
	 */
	public int getNumStates() {
		return this.states.size();
	}

	/**
	 * Adds a state to the fst
	 * 
	 * @param s the state to be added
	 * @return the state's index 
	 */
	public int AddState(State<W> s) {
		this.states.add(s);
		return states.size() - 1;
	}
	
	/**
	 * 
	 * @return the states array list
	 */
	public ArrayList<State<W>> getStates() {
		return this.states; 
	}
	
	/**
	 * 
	 * @param sIndex the state's index
	 * @return the state
	 */
	public State<W> getState(int sIndex) {
		if (sIndex<this.states.size()) {
			return this.states.get(sIndex);
		}
		
		return null;
	}

	/**
	 * 
	 * @param sIndex
	 * @param s
	 */
	public void setState(int sIndex, State<W> s) {
		if (sIndex < this.states.size()) {
			this.states.set(sIndex, s);
		}
	}

}
