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

/**
 * 
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 * 
 */
public class TropicalSemiring implements Semiring<Weight<Float>> {

	/**
	 * 
	 */
	public TropicalSemiring() {
		super();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.fst.weight.AbstractSemiring#Plus(edu.cmu.fst.weight.Weight,
	 * edu.cmu.fst.weight.Weight)
	 */
	@Override
	public Weight<Float> plus(Weight<Float> w1, Weight<Float> w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return null;
		}

		return w1.getValue() < w2.getValue() ? w1 : w2;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.fst.weight.AbstractSemiring#Times(edu.cmu.fst.weight.Weight,
	 * edu.cmu.fst.weight.Weight)
	 */
	@Override
	public Weight<Float> times(Weight<Float> w1, Weight<Float> w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return null;
		}

		return new Weight<Float>(w1.getValue() + w2.getValue());
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.fst.weight.AbstractSemiring#Divide(edu.cmu.fst.weight.Weight,
	 * edu.cmu.fst.weight.Weight)
	 */
	@Override
	public Weight<Float> divide(Weight<Float> w1, Weight<Float> w2) {
		if (!isMember(w1) || !isMember(w2)) {
			return null;
		}
		
		if(w2.equals(zero())) {
			return null;
		} else if (w1.equals(zero())) {
			return zero();
		}

		return new Weight<Float>(w1.getValue() - w2.getValue());
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.fst.weight.AbstractSemiring#zero()
	 */
	@Override
	public Weight<Float> zero() {
		return new Weight<Float>(Float.POSITIVE_INFINITY);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.fst.weight.AbstractSemiring#one()
	 */
	@Override
	public Weight<Float> one() {
		return new Weight<Float>(0.f);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.fst.weight.AbstractSemiring#isMember(edu.cmu.fst.weight.Weight)
	 */
	@Override
	public boolean isMember(Weight<Float> w) {
		return (w != null)                                          // The weight should not be null,
				&& (w.getValue() != null)                           // it must hold a valid value,
				&& (!Float.isNaN(w.getValue()))                     // not a NaN
				&& (!w.getValue().equals(Float.NEGATIVE_INFINITY)); // and different from -inf
	}
}
