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

import java.util.Vector;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Path {
	private Vector<String> path;
	
	private float cost;
	
	Path() {
		path = new Vector<String>();
		cost = 0.f;
	}

	/**
	 * @return the path
	 */
	public Vector<String> getPath() {
		return path;
	}

	/**
	 * @param path the path to set
	 */
	public void setPath(Vector<String> path) {
		this.path = path;
	}

	/**
	 * @return the cost
	 */
	public float getCost() {
		return cost;
	}

	/**
	 * @param cost the cost to set
	 */
	public void setCost(float cost) {
		this.cost = cost;
	}
	
	
}
