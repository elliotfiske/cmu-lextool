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
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.ImmutableFst;
import edu.cmu.sphinx.fst.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class ArcSort {
    private ArcSort() {
    }

    public static void apply(Fst fst, Comparator<Arc> cmp) {
      int numStates = fst.getNumStates();
      for (int i=0; i<numStates; i++) {
          State s = fst.getState(i);
            s.arcSort(cmp);
        }
    }

    public static void apply(ImmutableFst fst, Comparator<Arc> cmp) {
      int numStates = fst.getNumStates();
      for (int i=0; i<numStates; i++) {
          State s = fst.getState(i);
          s.arcSort(cmp);
      }
  }

}
