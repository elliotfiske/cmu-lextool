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

package edu.cmu.sphinx.fst.fst;

import static org.junit.Assert.assertTrue;

import org.junit.Test;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;

/**
 * 
 * Creates the example wfst depicted in Fig. 1 in paper:
 * 
 * C. Allauzen, M. Riley, J. Schalkwyk, W. Skut, M. Mohri,
 * "OpenFst: a general and efficient weighted finite-state transducer library",
 * Proceedings of the 12th International Conference on Implementation and
 * Application of Automata (CIAA 2007), pp. 11–23, Prague, Czech Republic, July
 * 2007.
 * 
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 * 
 */
public class FstTest {

    @Test
    public void testStateDeletions() {
        System.out.println("Testing State Deletions...");
        Fst fst1 = new Fst(null);
        Fst fst2 = new Fst(null);

        // State 0
        State s1 = new State(0.f);
        s1.setId(0);
        State s2 = new State(0.f);
        s2.setId(1);
        State s3 = new State(0.f);
        s3.setId(2);
        State s4 = new State(0.f);
        s4.setId(3);
        fst1.addState(s1);
        s1.addArc(new Arc(0, 0, 0.f, s2));
        s1.addArc(new Arc(0, 0, 0.f, s3));
        s1.addArc(new Arc(0, 0, 0.f, s4));
        fst1.setStart(s1);

        // State 1
        fst1.addState(s2);
        s2.addArc(new Arc(0, 0, 0.f, s3));

        // State 2
        fst1.addState(s3);
        s3.addArc(new Arc(0, 0, 0.f, s4));

        // State 3
        fst1.addState(s4);
        s4.setFinalWeight(1.f);

        // State 0
        s1 = new State(0.f);
        s1.setId(0);
        s2 = new State(0.f);
        s2.setId(1);
        s3 = new State(0.f);
        s3.setId(2);
        s4 = new State(0.f);
        s4.setId(3);
        fst2.addState(s1);
        s1.addArc(new Arc(0, 0, 0.f, s3));
        s1.addArc(new Arc(0, 0, 0.f, s4));
        fst2.setStart(s1);

        // State 2
        fst2.addState(s3);
        s3.addArc(new Arc(0, 0, 0.f, s4));

        // State 3
        fst2.addState(s4);
        s4.setFinalWeight(1.f);

        fst1.deleteState(fst1.getStates().get(1));

        assertTrue(fst1.equals(fst2));

        System.out.println("Testing State Deletions Completed!\n");
    }

}
