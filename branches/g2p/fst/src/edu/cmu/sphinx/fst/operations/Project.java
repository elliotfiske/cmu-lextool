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

	public static <T extends Comparable<T>> void apply(Fst<T> fst, ProjectType pType) {
		if(pType == ProjectType.INPUT) {
			fst.setOsyms(fst.getIsyms());
		} else if (pType == ProjectType.OUTPUT) {
			fst.setIsyms(fst.getOsyms());
		}

		State<T> s;
		Arc<T> arc;
		for(Iterator<State<T>> itS = fst.stateIterator(); itS.hasNext();) {
			s = itS.next();
			for(Iterator<Arc<T>> itA = s.arcIterator(); itA.hasNext();) {
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
