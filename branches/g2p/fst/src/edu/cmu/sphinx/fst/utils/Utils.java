/**
 * 
 */
package edu.cmu.sphinx.fst.utils;

import java.util.Vector;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Utils {

	public static Vector<String> split_string(String input, String delim) {
		Vector<String> res = new Vector<String>();
		
		int start = 0;
		int len = 0;
		int pos = 0;

		while (start < input.length()) {
			if (delim.isEmpty()) {
				len = 1;
			} else {
				pos = input.indexOf(delim, start);
				if (pos != -1) {
					len = pos - start;
				} else {
					len = input.length();
				}
			}
			res.add(input.substring(start, len));
			if (delim.isEmpty()) {
				start = start + len;
			} else {
				start = start + len + delim.length();
			}
		}
		
		
		return res;
	}

}
