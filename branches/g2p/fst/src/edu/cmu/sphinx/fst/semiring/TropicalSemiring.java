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

package edu.cmu.sphinx.fst.semiring;

import edu.cmu.sphinx.fst.utils.Utils;

/**
 * 
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 * 
 */
public class TropicalSemiring extends Semiring {

	private static final long serialVersionUID = 2711172386738607866L;
	
	private static double zero = Double.POSITIVE_INFINITY;
	private static double one = 0.;
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Plus(edu.cmu.sphinx.fst.weight.double,
	 * edu.cmu.sphinx.fst.weight.double)
	 */
	@Override
	public double plus(double w1, double w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return Double.NEGATIVE_INFINITY;
		}

		return w1 < w2 ? w1 : w2;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Times(edu.cmu.sphinx.fst.weight.double,
	 * edu.cmu.sphinx.fst.weight.double)
	 */
	@Override
	public double times(double w1, double w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return Double.NEGATIVE_INFINITY;
		}

		return Utils.round(w1 + w2, accuracy);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Divide(edu.cmu.sphinx.fst.weight.double,
	 * edu.cmu.sphinx.fst.weight.double)
	 */
	@Override
	public double divide(double w1, double w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return Double.NEGATIVE_INFINITY;
		}
		
		if(w2 == zero) {
			return Double.NEGATIVE_INFINITY;
		} else if (w1 == zero) {
			return zero;
		}

		return Utils.round(w1 - w2, accuracy);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#zero()
	 */
	@Override
	public double zero() {
		return zero;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#one()
	 */
	@Override
	public double one() {
		return one;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#isMember(edu.cmu.sphinx.fst.weight.double)
	 */
	@Override
	public boolean isMember(double w) {
		return (!Double.isNaN(w))                     // not a NaN
				&& (w != Double.NEGATIVE_INFINITY); // and different from -inf
	}

	@Override
	public double reverse(double w1) {
		return w1;
	}
}
