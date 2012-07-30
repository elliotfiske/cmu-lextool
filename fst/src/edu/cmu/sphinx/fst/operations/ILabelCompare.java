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

import edu.cmu.sphinx.fst.Arc;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * @param
 * @param
 * 
 */
public class ILabelCompare implements Comparator<Arc> {

    /*
     * (non-Javadoc)
     * 
     * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
     */
    @Override
    public int compare(Arc o1, Arc o2) {
        return (o1.getIlabel() < o2.getIlabel()) ? -1 : ((o1.getIlabel() == o2
                .getIlabel()) ? 0 : 1);
    }

}
