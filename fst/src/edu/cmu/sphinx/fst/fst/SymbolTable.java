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

package edu.cmu.sphinx.fst.fst;

import java.io.Serializable;

import com.google.common.collect.HashBiMap;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class SymbolTable implements Serializable {
	
	/**
	 * 
	 */
	private static final long serialVersionUID = -6780807561998593228L;
	
	
	private HashBiMap<Integer, String> map;

	public SymbolTable() {
		map = HashBiMap.create();
	}
	
	public String put(Integer key, String value) {
		return map.put(key, value); 
	}
	
	public String get(Integer key) {
		return map.get(key);
	}
	
	public Integer get(String key) {
		return map.inverse().get(key);
	}
	
	public int size() {
		return map.size();
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		SymbolTable other = (SymbolTable) obj;
		if (map == null) {
			if (other.map != null)
				return false;
		} else if (!map.equals(other.map))
			return false;
		return true;
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "SymbolTable [map=" + map + "]";
	}

	
}
