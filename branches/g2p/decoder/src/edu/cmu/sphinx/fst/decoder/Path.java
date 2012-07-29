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
import java.util.Iterator;

import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Path {
	private ArrayList<String> path;
	private double cost;
	private Semiring semiring;
	
	/**
	 * 
	 * @param path
	 * @param semiring
	 */
	public Path(ArrayList<String> path, Semiring semiring) {
		this.path = path;
		this.semiring = semiring;
		cost = this.semiring.zero();
	}

	public Path(Semiring semiring) {
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
	public double getCost() {
		return cost;
	}

	/**
	 * @param cost the cost to set
	 */
	public void setCost(double cost) {
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
		String s;
		for(Iterator<String> it = path.iterator(); it.hasNext();) {
			s = it.next();
			sb.append(s);
			
			if(it.hasNext()) {
				sb.append(" ");
			}
		}
		return sb.toString();
	}
}
