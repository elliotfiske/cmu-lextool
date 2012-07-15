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

import java.util.Comparator;

import edu.cmu.sphinx.fst.arc.Arc;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * @param <T>
 * @param <T>
 *
 */
public class OLabelCompare<T extends Comparable<T>> implements Comparator<Arc<T>> {

	/*
	 * (non-Javadoc)
	 * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
	 */
	@Override
	public int compare(Arc<T> o1, Arc<T> o2) {
		return (o1.getOlabel() < o2.getOlabel()) ? -1 : ((o1.getOlabel() == o2.getOlabel()) ? 0 : 1);
	}

}
