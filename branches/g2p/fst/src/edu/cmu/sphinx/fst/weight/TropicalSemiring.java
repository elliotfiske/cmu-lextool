/**
 * 
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

import edu.cmu.sphinx.fst.utils.Utils;

/**
 * 
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 * 
 */
public class TropicalSemiring extends AbstactSemiring<Double> {

	private static final long serialVersionUID = 2711172386738607866L;
	
	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Plus(edu.cmu.sphinx.fst.weight.Weight,
	 * edu.cmu.sphinx.fst.weight.Weight)
	 */
	@Override
	public Weight<Double> plus(Weight<Double> w1, Weight<Double> w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return null;
		}

		return w1.getValue() < w2.getValue() ? w1 : w2;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Times(edu.cmu.sphinx.fst.weight.Weight,
	 * edu.cmu.sphinx.fst.weight.Weight)
	 */
	@Override
	public Weight<Double> times(Weight<Double> w1, Weight<Double> w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return null;
		}

		return new Weight<Double>(Utils.round(w1.getValue() + w2.getValue(), accuracy));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Divide(edu.cmu.sphinx.fst.weight.Weight,
	 * edu.cmu.sphinx.fst.weight.Weight)
	 */
	@Override
	public Weight<Double> divide(Weight<Double> w1, Weight<Double> w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return null;
		}
		
		if(w2.equals(zero())) {
			return null;
		} else if (w1.equals(zero())) {
			return zero();
		}

		return new Weight<Double>(Utils.round(w1.getValue() - w2.getValue(), accuracy));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#zero()
	 */
	@Override
	public Weight<Double> zero() {
		return new Weight<Double>(Double.POSITIVE_INFINITY);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#one()
	 */
	@Override
	public Weight<Double> one() {
		return new Weight<Double>(0.);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#isMember(edu.cmu.sphinx.fst.weight.Weight)
	 */
	@Override
	public boolean isMember(Weight<Double> w) {
		return (w != null)                                          // The weight should not be null,
				&& (w.getValue() != null)                           // it must hold a valid value,
				&& (!Double.isNaN(w.getValue()))                     // not a NaN
				&& (!w.getValue().equals(Float.NEGATIVE_INFINITY)); // and different from -inf
	}

	@Override
	public Weight<Double> reverse(Weight<Double> w1) {
		return w1;
	}
}
