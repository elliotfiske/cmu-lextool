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

package edu.cmu.sphinx.fst.weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <T>
 */
public class Weight<T> {
	private Class<T> type;
	private T value;

	/**
	 * 
	 * @param value
	 */
	@SuppressWarnings("unchecked")
	public Weight(T value) {
		this.value = value;
		this.type = (Class<T>) value.getClass();
	}

	/**
	 * 
	 * @return
	 */
	public Class<T> getType() {
		return type;
	}

	/**
	 * 
	 * @param value
	 */
	public void setValue(T value) {
		this.value = value;
	}

	/**
	 * 
	 * @return
	 */
	public T getValue() {
		return (T) value;
	}

	/*
	 * (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@SuppressWarnings("unchecked")
	@Override
	public boolean equals(Object obj) {
		if(!(obj instanceof Weight<?>)) {
			return false;
		}
		return this.getValue().equals( ((Weight<T>) obj).getValue());
	}
	
	/*
	 * (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		String res = "Weight<"+type.getSimpleName()+">: " + value;
		return res;
	}

}
