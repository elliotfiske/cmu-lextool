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
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class State<T extends Comparable<T>> implements Serializable {
	
	private static final long serialVersionUID = -405571712671111894L;
	
	// Id
	private String id;

	// Final weight
	private Weight<T> fnlWeight;

	// Outgoing arcs collection 
	private ArrayList<Arc<T>> arcs = new ArrayList<Arc<T>>();

	// holds the semiring
	private Semiring<T> semiring;
	
	/**
	 * 
	 */
	public State(T weight) {
		fnlWeight = new Weight<T>(weight);
	}
	
	public void arcSort(Comparator<Arc<T>> cmp) {
		Collections.sort(arcs, cmp);
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
	 * @return the id
	 */
	public String getId() {
		return id;
	}

	/**
	 * @param id the id to set
	 */
	public void setId(String id) {
		this.id = id;
	}

	/**
	 * @return the semiring
	 */
	public Semiring<T> getSemiring() {
		return semiring;
	}

	/**
	 * @param semiring the semiring to set
	 */
	public void setSemiring(Semiring<T> semiring) {
		this.semiring = semiring;
	}

	/**
	 * 
	 * @return the {@link #addArc(Arc)}of outgoing arcs
	 */
	public int getNumArcs() {
		return this.arcs.size();
	}
	
	/**
	 * Adds an arc
	 * 
	 * @param arc the arc to add
	 * @return the arc's index
	 */
	public void addArc(Arc<T> arc) {
		if(this.semiring != null) {
			// Check if there is already an arc with same input/output labels and nextstate
			for (Iterator<Arc<T>> it = arcs.iterator(); it.hasNext();) {
				Arc<T> oldArc = it.next();
				if((oldArc.getIlabel() == arc.getIlabel()) &&
						(oldArc.getOlabel() == arc.getOlabel()) &&
						(oldArc.getNextStateId().equals(arc.getNextStateId()))) {
					//newArc = oldArc.copy();
					oldArc.setWeight(semiring.plus(oldArc.getWeight(), arc.getWeight()));
					return;
				}
			}
		} else {
			System.out.println(("Semiring is null. Merging of arcs will not happen"));
		}

		this.arcs.add(arc);
	}
	

	/**
	 * 
	 * @param aIndex the arc's index
	 * @return the arc
	 */
	public Arc<T> getArc(int index) {
			return this.arcs.get(index);
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
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("("+id+ ", "+ fnlWeight+")");
		return sb.toString();
//		return id;
	}

	// delete an arc
	public Arc<T> deleteArc(int index) {
		return this.arcs.remove(index);
	}

	public boolean isFinal() {
		if(semiring == null) {
			System.out.println(("Semiring is null. Cannot determine if state is final."));
			return false;
		}
		return !this.fnlWeight.equals(semiring.zero());
	}	
}
