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

import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <W>
 */
public class Arc<T extends Comparable<T>> {
	
	// Arc's weight
	protected Weight<T> weight;

	// input label
	protected int iLabel;
	
	// output label 
	protected int oLabel;
	
	// next state's id
	protected String nextStateId;


	public Arc() {}
	/**
	 * Creates a new arc
	 * 
	 * @param iLabel the input label's id
	 * @param oLabel the output label's id
	 * @param weight  the arc's weight
	 * @param nextStateId the next state's id
	 */
	public Arc(int iLabel, int oLabel, Weight<T> weight, String nextStateId) {
		this.weight = weight;
		this.iLabel = iLabel;
		this.oLabel = oLabel;
		this.nextStateId = nextStateId;
	}

	/**
	 * Creates a new arc
	 * 
	 * @param iLabel the input label's id
	 * @param oLabel the output label's id
	 * @param weightValue the arc's weight value
	 * @param nextStateId the next state's id
	 */
	public Arc(int iLabel, int oLabel, T weightValue, String nextStateId) {
		this(iLabel, oLabel, new Weight<T>(weightValue), nextStateId);
	}
	
	/**
	 * Get the arc's weight
	 * 
	 * @return the weight
	 */
	public Weight<T> getWeight() {
		return weight;
	}


	/**
	 * Set the arc's weight
	 * 
	 * @param weight the weight to set
	 */
	public void setWeight(Weight<T> weight) {
		this.weight = weight;
	}

	/**
	 * Get the input label's id
	 * 
	 * @return the input label's id
	 */
	public int getIlabel() {
		return iLabel;
	}

	/**
	 * Set the input label's id
	 * 
	 * @param iLabel the input label's to set
	 */
	public void setIlabel(int iLabel) {
		this.iLabel = iLabel;
	}

	/**
	 * Get the output label's id
	 * 
	 * @return the output label's id
	 */
	public int getOlabel() {
		return oLabel;
	}

	/**
	 * Set the output label's id
	 * 
	 * @param oLabel the output label's to set
	 */
	public void setOlabel(int oLabel) {
		this.oLabel = oLabel;
	}

	/**
	 * Get the next state's id
	 * 
	 * @return the next state's id
	 */
	public String getNextStateId() {
		return nextStateId;
	}

	/**
	 * Set the next state's id
	 * 
	 * @param nextStateId the next state's id to set
	 */
	public void setNextState(String nextStateId) {
		this.nextStateId = nextStateId;
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
		if (!nextStateId.equals(other.nextStateId))
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
		return "(" + iLabel + ", " + oLabel + ", " + weight + ", " + nextStateId + ")";
	}

//	@Override
//	public void writeExternal(ObjectOutput out) throws IOException {
//		out.writeInt(iLabel);
//		out.writeInt(oLabel);
//		out.writeObject(weight);
//		out.writeObject(nextStateId);
//	}
//
//	@SuppressWarnings("unchecked")
//	@Override
//	public void readExternal(ObjectInput in) throws IOException, ClassNotFoundException {
//		iLabel = in.readInt();
//		oLabel = in.readInt();
//		weight = (Weight<T>) in.readObject();
//		nextStateId = (String) in.readObject();
//	}
}
