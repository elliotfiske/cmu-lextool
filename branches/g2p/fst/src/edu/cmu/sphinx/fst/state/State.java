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


package edu.cmu.sphinx.fst.state;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class State<T> implements Serializable {
	
	private static final long serialVersionUID = -405571712671111894L;

	// Final weight
	Weight<T> fnlWeight;

	// Outgoing arcs collection 
	ArrayList<Arc<T>> arcs = new ArrayList<Arc<T>>();
	
	// # of input epsilons
	int iEpsilons = 0;
	
	// # of output epsilons
	int pEpsilons = 0;

	/**
	 * 
	 */
	public State(T weight) {
		fnlWeight = new Weight<T>(weight);
	}

	/**
	 * 
	 * @param fnlWeight
	 */
	public State(Weight<T> fnlWeight) {
		this.fnlWeight = fnlWeight;
	}

	/**
	 * @return the Final weight
	 */
	public Weight<T> getFinalWeight() {
		return fnlWeight;
	}

	/**
	 * @param fnlWeight the Final weight to set
	 */
	public void setFinalWeight(Weight<T> fnlWeight) {
		this.fnlWeight = fnlWeight;
	}

	/**
	 * @return the arcs
	 */
	public ArrayList<Arc<T>> getArcs() {
		return arcs;
	}

	/**
	 * @return the # of input epsilons
	 */
	public int getiEpsilons() {
		return iEpsilons;
	}

	/**
	 * @return the # of output epsilons
	 */
	public int getpEpsilons() {
		return pEpsilons;
	}
	
	/**
	 * 
	 * @return the {@link #AddArc(Arc)}of outgoing arcs
	 */
	public int getNumArcs() {
		return this.arcs.size();
	}
	
	/**
	 * Adds an arc
	 * 
	 * @param a the arc to add
	 * @return the arc's index
	 */
	public int AddArc(Arc<T> a) {
		this.arcs.add(a);
		return this.arcs.size() - 1; 
		
	}
	
	/**
	 * 
	 * @param aIndex
	 * @param a
	 */
	public void setArc(int aIndex, Arc<T> a) {
		if (aIndex < this.arcs.size()) {
			this.arcs.set(aIndex, a);
		}
	}

	/**
	 * 
	 * @param aIndex the arc's index
	 * @return the arc
	 */
	public Arc<T> getArc(int aIndex) {
		if (aIndex<this.arcs.size()) {
			return this.arcs.get(aIndex);
		}
		
		return null;
	}
	
	public void arcSort(Comparator<Arc<T>> cmp) {
		Collections.sort(arcs, cmp);
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		@SuppressWarnings("unchecked")
		State<T> other = (State<T>) obj;
		if (arcs == null) {
			if (other.arcs != null)
				return false;
		} else if (!arcs.equals(other.arcs))
			return false;
		if (fnlWeight == null) {
			if (other.fnlWeight != null)
				return false;
		} else if (!fnlWeight.equals(other.fnlWeight))
			return false;
		if (iEpsilons != other.iEpsilons)
			return false;
		if (pEpsilons != other.pEpsilons)
			return false;
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "State [fnlWeight=" + fnlWeight + ", arcs=" + arcs
				+ ", iEpsilons=" + iEpsilons + ", pEpsilons=" + pEpsilons + "]";
	}
	
	
	
}
