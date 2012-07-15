/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Project {
	private Project() {}

	public static <T extends Comparable<T>> void apply(Fst<T> fst, ProjectType pType) {
		if(pType == ProjectType.INPUT) {
			fst.setOsyms(fst.getIsyms());
		} else if (pType == ProjectType.OUTPUT) {
			fst.setIsyms(fst.getOsyms());
		}

		for(int i=0; i<fst.getNumStates(); i++) {
			State<T> s = fst.getStateByIndex(i);
			for(int j=0; j<s.getNumArcs(); j++) {
				Arc<T> arc = s.getArc(j);
				
				if(pType == ProjectType.INPUT) {
					arc.setOlabel(arc.getIlabel());
				} else if (pType == ProjectType.OUTPUT) {
					arc.setIlabel(arc.getOlabel());
				}
			}
		}
		
	}
}
