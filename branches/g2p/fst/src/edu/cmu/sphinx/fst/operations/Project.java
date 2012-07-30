/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Project {
    private Project() {
    }

    public static void apply(Fst fst, ProjectType pType) {
        if (pType == ProjectType.INPUT) {
            fst.setOsyms(fst.getIsyms());
        } else if (pType == ProjectType.OUTPUT) {
            fst.setIsyms(fst.getOsyms());
        }

        for (State s : fst.getStates()) {
            for (Arc a : s.getArcs()) {
                if (pType == ProjectType.INPUT) {
                    a.setOlabel(a.getIlabel());
                } else if (pType == ProjectType.OUTPUT) {
                    a.setIlabel(a.getOlabel());
                }
            }
        }
    }
}
