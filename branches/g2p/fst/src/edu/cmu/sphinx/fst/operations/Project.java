/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Project {
	private Project() {}

	public static void apply(Fst fst, ProjectType pType) {
		if(pType == ProjectType.INPUT) {
			fst.setOsyms(fst.getIsyms());
		} else if (pType == ProjectType.OUTPUT) {
			fst.setIsyms(fst.getOsyms());
		}

		State s;
		Arc arc;
		for(Iterator<State> itS = fst.stateIterator(); itS.hasNext();) {
			s = itS.next();
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				arc = itA.next();
				if(pType == ProjectType.INPUT) {
					arc.setOlabel(arc.getIlabel());
				} else if (pType == ProjectType.OUTPUT) {
					arc.setIlabel(arc.getOlabel());
				}
			}
		}
		
	}
}
