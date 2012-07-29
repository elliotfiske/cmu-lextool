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

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class State {
	
	// Id
	private String id;

	// Final weight
	private double fnlWeight;

	// Outgoing arcs collection 
	private ArrayList<Arc> arcs = new ArrayList<Arc>();

	// holds the semiring
	private Semiring semiring;
	
	
	public State() {}
	/**
	 * 
	 */
	
	public void arcSort(Comparator<Arc> cmp) {
		Collections.sort(arcs, cmp);
	}

	/**
	 * 
	 * @param fnlWeight
	 */
	public State(double fnlWeight) {
		this.fnlWeight = fnlWeight;
	}
	
	/**
	 * @return the Final weight
	 */
	public double getFinalWeight() {
		return fnlWeight;
	}

	/**
	 * @return the arcs
	 */
	public ArrayList<Arc> getArcs() {
		return arcs;
	}
	/**
	 * @param arcs the arcs to set
	 */
	public void setArcs(ArrayList<Arc> arcs) {
		this.arcs = arcs;
	}
	/**
	 * @param fnldouble the Final weight to set
	 */
	public void setFinalWeight(double fnldouble) {
		this.fnlWeight = fnldouble;
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
	public Semiring getSemiring() {
		return semiring;
	}

	/**
	 * @param semiring the semiring to set
	 */
	public void setSemiring(Semiring semiring) {
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
	 * @return the arcs
	 */
	public Iterator<Arc> arcIterator() {
		return arcs.iterator();
	}
	
	/**
	 * Adds an arc
	 * 
	 * @param arc the arc to add
	 * @return the arc's index
	 */
	public void addArc(Arc arc) {
		if(this.semiring != null) {
			// Check if there is already an arc with same input/output labels and nextstate
			Arc oldArc;
			for (Iterator<Arc> it = arcs.iterator(); it.hasNext();) {
				oldArc = it.next();
				if((oldArc.getIlabel() == arc.getIlabel()) &&
						(oldArc.getOlabel() == arc.getOlabel()) &&
						(oldArc.getNextStateId().equals(arc.getNextStateId()))) {
					oldArc.setWeight(semiring.plus(oldArc.getWeight(), arc.getWeight()));
					return;
				}
			}
		} else {
			System.err.println(("Semiring is null. Merging of arcs will not happen"));
		}

		this.arcs.add(arc);
	}
	

	/**
	 * 
	 * @param aIndex the arc's index
	 * @return the arc
	 */
	public Arc getArc(int index) {
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
		State other = (State) obj;
		if (id == null) {
			if (other.id != null)
				return false;
		} else if (!id.equals(other.id))
			return false;
		if (arcs == null) {
			if (other.arcs != null)
				return false;
		} else if (!arcs.equals(other.arcs))
			return false;
		if (fnlWeight != other.fnlWeight)
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
	}

	// delete an arc
	public Arc deleteArc(int index) {
		return this.arcs.remove(index);
	}

	public boolean isFinal() {
		if(semiring == null) {
			System.err.println(("Semiring is null. Cannot determine if state is final."));
			return false;
		}
		return this.fnlWeight != semiring.zero();
	}

}
