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

import java.io.Serializable;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <T>
 */
public class Weight<T> implements Serializable {
	private static final long serialVersionUID = 6897737261832081990L;
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
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		String res = "Weight<"+type.getSimpleName()+">: " + value;
		return res;
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
		@SuppressWarnings("unchecked")
		Weight<T> other = (Weight<T>) obj;
		if (value == null) {
			if (other.value != null)
				return false;
		} else if (!value.equals(other.value))
			return false;
		return true;
	}
	
	

}
