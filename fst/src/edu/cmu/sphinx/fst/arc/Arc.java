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


package edu.cmu.sphinx.fst.arc;

import java.io.Serializable;

import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <W>
 */
public class Arc<T> implements Serializable{
	
	private static final long serialVersionUID = -7996802366816336109L;
	
	// Arc's weight
	protected Weight<T> weight;
	
	// input label
	protected int iLabel;
	
	// output label 
	protected int oLabel;
	
	// next state
	protected int nextState;

	/**
	 * 
	 * @param weight
	 * @param iLabel
	 * @param oLabel
	 * @param nextState
	 */
	public Arc(Weight<T> weight, int iLabel, int oLabel, int nextState) {
		this.weight = weight;
		this.iLabel = iLabel;
		this.oLabel = oLabel;
		this.nextState = nextState;
	}

	/**
	 * @return the weight
	 */
	public Weight<T> getWeight() {
		return weight;
	}

	/**
	 * @param weight the weight to set
	 */
	public void setWeight(Weight<T> weight) {
		this.weight = weight;
	}

	/**
	 * @return the inout label
	 */
	public int getIlabel() {
		return iLabel;
	}

	/**
	 * @param iLabel the input label to set
	 */
	public void setIlabel(int iLabel) {
		this.iLabel = iLabel;
	}

	/**
	 * @return the output label
	 */
	public int getOlabel() {
		return oLabel;
	}

	/**
	 * @param oLabel the output label to set
	 */
	public void setOlabel(int oLabel) {
		this.oLabel = oLabel;
	}

	/**
	 * @return the next state
	 */
	public int getNextState() {
		return nextState;
	}

	/**
	 * @param nextState the next state to set
	 */
	public void setNextState(int nextState) {
		this.nextState = nextState;
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
		Arc<T> other = (Arc<T>) obj;
		if (iLabel != other.iLabel)
			return false;
		if (nextState != other.nextState)
			return false;
		if (oLabel != other.oLabel)
			return false;
		if (weight == null) {
			if (other.weight != null)
				return false;
		} else if (!weight.equals(other.weight))
			return false;
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "Arc [weight=" + weight + ", iLabel=" + iLabel + ", oLabel="
				+ oLabel + ", nextState=" + nextState + "]";
	}
}
