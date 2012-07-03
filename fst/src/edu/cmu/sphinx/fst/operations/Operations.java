/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.Iterator;


import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Operations {
	public static <T> void ArcSort(Fst<T> fst, Comparator<Arc<T>> cmp) {
		ArrayList<State<T>> states = fst.getStates();
		Iterator<State<T>> itr = states.iterator();
	    
		while (itr.hasNext()){
	    	State<T> s = (State<T>) itr.next();
	    	s.arcSort(cmp);
	    }
	}

}
