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

import java.util.Comparator;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class PathComparator implements Comparator<Path> {

	@Override
	public int compare(Path o1, Path o2) {
		if(o1.getCost() < o2.getCost()) 
			return -1;
		else if(o1.getCost() > o2.getCost())
			return 1;
		
		return 0;
	}

}
