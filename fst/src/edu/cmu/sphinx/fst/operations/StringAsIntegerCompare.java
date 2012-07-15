/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.Comparator;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class StringAsIntegerCompare implements Comparator<String> {

	@Override
	public int compare(String o1, String o2) {
		if (o1 == null) {
			return -1;
		} if(o2 == null) {
			return 1; 
		}
		Integer val1 = null;
		Integer val2 = null;
		try {
			val1 = Integer.parseInt(o1);
		} catch (NumberFormatException e) {
			return ((java.lang.String) o1).compareTo(o2);
		}
		try {
			val2 = Integer.parseInt((java.lang.String) o2);
		} catch (NumberFormatException e) {
			return ((java.lang.String) o1).compareTo(o2);
		}
		return val1.compareTo(val2);
	}

}
