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

package edu.cmu.sphinx.fst.decoder;

import java.util.ArrayList;

import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Path<T extends Comparable<T>> {
	private ArrayList<String> path;
	private Weight<T> cost;
	private Semiring<T> semiring;
	
	/**
	 * 
	 * @param path
	 * @param semiring
	 */
	public Path(ArrayList<String> path, Semiring<T> semiring) {
		this.path = path;
		this.semiring = semiring;
		cost = this.semiring.zero();
	}

	public Path(Semiring<T> semiring) {
		this(new ArrayList<String>(), semiring);
	}

	/**
	 * @return the path
	 */
	public ArrayList<String> getPath() {
		return path;
	}

	/**
	 * @return the cost
	 */
	public Weight<T> getCost() {
		return cost;
	}

	/**
	 * @param cost the cost to set
	 */
	public void setCost(Weight<T> cost) {
		this.cost = cost;
	}

	/**
	 * @param path the path to set
	 */
	public void setPath(ArrayList<String> path) {
		this.path = path;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append(cost + "\t");
		for(int i=0; i<path.size();i++) {
			sb.append(path.get(i));
			if(i<path.size() - 1) {
				sb.append(" ");
			}
		}
		return sb.toString();
	}
	
	
}
