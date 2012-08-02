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

import static org.junit.Assert.assertTrue;

import java.util.HashMap;

import org.junit.Test;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.semiring.TropicalSemiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class ProjectTest {
    /**
     * Create an fst to Project as per the example at
     * http://www.openfst.org/twiki/bin/view/FST/ProjectDoc
     * 
     * @return the created fst
     */
    private Fst createFst() {
        TropicalSemiring ts = new TropicalSemiring();
        Fst fst = new Fst(ts);

        HashMap<String, Integer> isyms = new HashMap<String, Integer>();
        isyms.put("<eps>", 0);
        isyms.put("a", 1);
        isyms.put("b", 2);
        isyms.put("d", 3);
        isyms.put("c", 4);
        isyms.put("f", 5);

        HashMap<String, Integer> osyms = new HashMap<String, Integer>();
        osyms.put("<eps>", 0);
        osyms.put("u", 1);
        osyms.put("w", 2);
        osyms.put("v", 3);
        osyms.put("x", 4);
        osyms.put("y", 5);

        // State 0
        State s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(1, 5, 1.f, 1));
        s.addArc(new Arc(2, 4, 3.f, 1));
        fst.setStart(s.getId());

        // State 1
        s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(3, 3, 7.f, 1));
        s.addArc(new Arc(4, 2, 5.f, 2));

        // State 2
        s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(5, 1, 9.f, 3));

        // State 3
        s = new State(2.f);
        fst.addState(s);

        fst.setIsyms(isyms);
        fst.setOsyms(osyms);

        return fst;
    }

    /**
     * Create the Project on Input Labels as per the example at
     * http://www.openfst.org/twiki/bin/view/FST/ProjectDoc
     * 
     * @return the created fst
     */
    private Fst createPi() {
        TropicalSemiring ts = new TropicalSemiring();
        Fst fst = new Fst(ts);

        HashMap<String, Integer> syms = new HashMap<String, Integer>();
        syms.put("<eps>", 0);
        syms.put("a", 1);
        syms.put("b", 2);
        syms.put("d", 3);
        syms.put("c", 4);
        syms.put("f", 5);

        // State 0
        State s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(1, 1, 1.f, 1));
        s.addArc(new Arc(2, 2, 3.f, 1));
        fst.setStart(s.getId());

        // State 1
        s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(3, 3, 7.f, 1));
        s.addArc(new Arc(4, 4, 5.f, 2));

        // State 2
        s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(5, 5, 9.f, 3));

        // State 3
        s = new State(2.f);
        fst.addState(s);

        fst.setIsyms(syms);
        fst.setOsyms(syms);

        return fst;
    }

    /**
     * Create the Project on Output Labels as per the example at
     * http://www.openfst.org/twiki/bin/view/FST/ProjectDoc
     * 
     * @return the created fst
     */
    private Fst createPo() {
        TropicalSemiring ts = new TropicalSemiring();
        Fst fst = new Fst(ts);

        HashMap<String, Integer> syms = new HashMap<String, Integer>();
        syms.put("<eps>", 0);
        syms.put("u", 1);
        syms.put("w", 2);
        syms.put("v", 3);
        syms.put("x", 4);
        syms.put("y", 5);

        // State 0
        State s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(5, 5, 1.f, 1));
        s.addArc(new Arc(4, 4, 3.f, 1));
        fst.setStart(s.getId());

        // State 1
        s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(3, 3, 7.f, 1));
        s.addArc(new Arc(2, 2, 5.f, 2));

        // State 2
        s = new State(ts.zero());
        fst.addState(s);
        s.addArc(new Arc(1, 1, 9.f, 3));

        // State 3
        s = new State(2.f);
        fst.addState(s);

        fst.setIsyms(syms);
        fst.setOsyms(syms);
        return fst;
    }

    @Test
    public void testProject() {
        System.out.println("Testing Project...");
        // Project on Input label
        Fst fst = createFst();
        Fst p = createPi();
        Project.apply(fst, ProjectType.INPUT);
        assertTrue(fst.equals(p));

        // Project on Output label
        fst = createFst();
        p = createPo();
        Project.apply(fst, ProjectType.OUTPUT);
        assertTrue(fst.equals(p));

        System.out.println("Testing Project Completed!\n");

    }

}
