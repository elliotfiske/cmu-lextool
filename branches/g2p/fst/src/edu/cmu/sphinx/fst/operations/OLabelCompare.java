/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.Comparator;

import edu.cmu.sphinx.fst.arc.Arc;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * @param <T>
 * @param <T>
 *
 */
public class OLabelCompare<T> implements Comparator<Arc<T>> {

	/*
	 * (non-Javadoc)
	 * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
	 */
	@Override
	public int compare(Arc<T> o1, Arc<T> o2) {
		return (o1.getoLabel() < o2.getoLabel()) ? -1 : ((o1.getoLabel() == o2.getoLabel()) ? 0 : 1);
	}

}
