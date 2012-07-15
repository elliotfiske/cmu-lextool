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
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 * @param <T>
 *
 */
public interface Semiring<T extends Comparable<T>> {
	static final int accuracy = 5; 
	
	/**
	 * 
	 * @param w1
	 * @param w2
	 * @return
	 */
	public abstract Weight<T> plus(Weight<T> w1, Weight<T> w2);
	
	
	public abstract Weight<T> reverse(Weight<T> w1);
	
	/**
	 * 
	 * @param w1
	 * @param w2
	 * @return
	 */
	public abstract Weight<T> times(Weight<T> w1, Weight<T> w2);
	
	/**
	 * 
	 * @param w1
	 * @param w2
	 * @return
	 */
	public abstract Weight<T> divide(Weight<T> w1, Weight<T> w2);
	
	/**
	 * 
	 * @return
	 */
	public abstract Weight<T> zero();
	
	/**
	 * 
	 * @return
	 */
	public abstract Weight<T> one();

	/**
	 * 
	 * @param w
	 * @return
	 */
	public abstract boolean isMember(Weight<T> w);
	
	
	public boolean naturalLess(Weight<T> w1, Weight<T> w2);
	
	
}