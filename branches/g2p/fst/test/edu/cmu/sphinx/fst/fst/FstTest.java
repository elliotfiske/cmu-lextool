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
        State s = new State(0.f);
        fst1.addState(s);
        s.addArc(new Arc(0, 0, 0.f, 1));
        s.addArc(new Arc(0, 0, 0.f, 2));
        s.addArc(new Arc(0, 0, 0.f, 3));
        fst1.setStart(s.getId());

        // State 1
        s = new State(0.f);
        fst1.addState(s);
        s.addArc(new Arc(0, 0, 0.f, 2));

        // State 2
        s = new State(0.f);
        fst1.addState(s);
        s.addArc(new Arc(0, 0, 0.f, 3));

        // State 3
        s = new State(0.f);
        fst1.addState(s);
        fst1.setFinal(s.getId(), 1.f);

        // State 0
        s = new State(0.f);
        s.setId(0);
        fst2.addState(s);
        s.addArc(new Arc(0, 0, 0.f, 2));
        s.addArc(new Arc(0, 0, 0.f, 3));
        fst2.setStart(s.getId());

        // State 2
        s = new State(0.f);
        s.setId(2);
        fst2.addState(s);
        s.addArc(new Arc(0, 0, 0.f, 3));

        // State 3
        s = new State(0.f);
        s.setId(3);
        fst2.addState(s);
        fst2.setFinal(s.getId(), 1.f);

        fst1.deleteState(1);

        assertTrue(fst1.equals(fst2));

        System.out.println("Testing State Deletions Completed!\n");
    }

}
