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

import java.util.ArrayList;
import java.util.HashMap;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Utils {

    public static ArrayList<String> split_string(String input, String delim) {
        ArrayList<String> res = new ArrayList<String>(input.length());

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

    public static int search(ArrayList<String> src, ArrayList<String> pattern,
            int start) {
        int index = -1;
        int pos = -1;
        int startpos = 0;
        if (start > src.size() - pattern.size()) {
            return -1;
        }

        do {
            pos = src
                    .subList(startpos + start, src.size() - pattern.size() + 1)
                    .indexOf(pattern.get(0));
            if (pos == -1) {
                return pos;
            }

            boolean flag = true;
            for (int i = 1; i < pattern.size(); i++) {
                if (!src.get(startpos + start + pos + i).equals(pattern.get(i))) {
                    index = -1;
                    flag = false;
                    break;
                }
            }

            if (flag) {
                index = startpos + pos;
                break;
            } else {
                startpos += pos + 1;
            }
        } while (startpos + start < src.size());

        return index;
    }

    public static float round(float value, int digits) {
        if (Float.isInfinite(value) || Float.isNaN(value)) {
            return value;
        }
        return (float) (Math.round(value * Math.pow(10, digits)) / Math.pow(10,
                digits));
    }
    
    public static int getIndex(String[] isyms, String string) {
        for(int i=0; i<isyms.length; i++) {
            if(string.equals(isyms[i])) {
                return i;
            }
        }
        return -1;
    }

    public static String[] toStringArray(HashMap<String, Integer> syms) {
        String[] res = new String[syms.size()];
        for(String sym: syms.keySet()) {
            res[syms.get(sym)] = sym;
        }
        return res;
    }

}
