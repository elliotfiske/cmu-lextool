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

	public static int search(Vector<String> src, Vector<String> pattern, int start) {
		int index = -1;
		int pos = -1;
		int startpos =0;
		if(start > src.size() - pattern.size()) {
			return -1;
		}
		
		do {
			pos = src.subList(startpos + start, src.size()-pattern.size()+1).indexOf(pattern.get(0));
			if (pos == -1){
				return pos;
			}
				
			boolean flag = true;
			for(int i = 1; i< pattern.size(); i++) {
				if(!src.get(startpos + start+pos+i).equals(pattern.get(i))) {
					index = -1;
					flag = false;
					break;
				}
			}
			
			if(flag) {
				index = startpos + pos;
				break;
			} else {
				startpos += pos + pattern.size();
			}
		} while (startpos + start < src.size());
		
		return index;
	}

	
	public static Double round(Double value, int digits) {
		if(Double.isInfinite(value) || Double.isNaN(value)) {
			return value;
		}
		Double res = new Double((double) Math.round(value * Math.pow(10, digits)) / Math.pow(10, digits));
		return res;
	}


}
