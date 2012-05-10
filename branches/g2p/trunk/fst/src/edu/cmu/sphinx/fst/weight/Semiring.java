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
public interface Semiring<W extends Weight<?>> {
	/**
	 * 
	 * @param w1
	 * @param w2
	 * @return
	 */
	public abstract W plus(W w1, W w2);
	
	/**
	 * 
	 * @param w1
	 * @param w2
	 * @return
	 */
	public abstract W times(W w1, W w2);
	
	/**
	 * 
	 * @param w1
	 * @param w2
	 * @return
	 */
	public abstract W divide(W w1, W w2);
	
	/**
	 * 
	 * @return
	 */
	public abstract W zero();
	
	/**
	 * 
	 * @return
	 */
	public abstract W one();

	/**
	 * 
	 * @param w
	 * @return
	 */
	public abstract boolean isMember(W w);
}