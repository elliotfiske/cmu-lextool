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
public class ProbabilitySemiring extends AbstactSemiring<Double> {

	/**
	 * 
	 */
	private static final long serialVersionUID = 5592668313009971909L;

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

		return new Weight<Double>(Utils.round(w1.getValue() + w2.getValue(), accuracy));
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

		return new Weight<Double>(Utils.round(w1.getValue() * w2.getValue(), accuracy));
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#Divide(edu.cmu.sphinx.fst.weight.Weight,
	 * edu.cmu.sphinx.fst.weight.Weight)
	 */
	@Override
	public Weight<Double> divide(Weight<Double> w1, Weight<Double> w2) {
		// TODO Auto-generated method stub
		return null;
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#zero()
	 */
	@Override
	public Weight<Double> zero() {
		return new Weight<Double>(0.);
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see edu.cmu.sphinx.fst.weight.AbstractSemiring#one()
	 */
	@Override
	public Weight<Double> one() {
		return new Weight<Double>(1.);
	}

	/*
	 * (non-Javadoc)
	 * @see edu.cmu.sphinx.fst.weight.Semiring#isMember(edu.cmu.sphinx.fst.weight.Weight)
	 */
	@Override
	public boolean isMember(Weight<Double> w) {
		return (w != null)                                          // The weight should not be null,
				&& (w.getValue() != null)                           // it must hold a valid value,
				&& (!Double.isNaN(w.getValue()))                    // not a NaN,
				&& (w.getValue()>=0);                     			// and positive
	}

	@Override
	public Weight<Double> reverse(Weight<Double> w1) {
		// TODO: ???
		System.out.println("Not Implemented");
		return null;
	}

}
