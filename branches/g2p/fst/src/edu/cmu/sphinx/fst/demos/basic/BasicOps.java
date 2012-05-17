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


package edu.cmu.sphinx.fst.demos.basic;

import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;


/**
 * 
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 *
 */
public class BasicOps {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		TropicalSemiring s = new TropicalSemiring();
		Weight<Float> w1 = new Weight<Float>(0.1f);
		Weight<Float> w2 = new Weight<Float>(0.5f);
		Weight<Float> one = s.one();
		Weight<Float> zero = s.zero();
		
		
		System.out.println("w1: " + w1);
		System.out.println("w2: " + w2);
		System.out.println("one: " + s.one());
		System.out.println("zero: " + s.zero());

		System.out.println("Plus(w1, w2): " + s.plus(w1, w2));
		System.out.println("Plus(w2, w1): " + s.plus(w2, w1));
		System.out.println("Plus(w1, zero): " + s.plus(w1, zero));
		System.out.println("Plus(zero, w1): " + s.plus(zero, w1));
		System.out.println("Plus(w1, one): " + s.plus(w1, one));
		System.out.println("Plus(one, w1): " + s.plus(one, w1));
		System.out.println("Plus(zero, one): " + s.plus(zero, one));
		System.out.println("Plus(one, zero): " + s.plus(one, zero));
		System.out.println("Plus(zero, zero): " + s.plus(zero, zero));
		System.out.println("Plus(one, one): " + s.plus(one, one));
		
		System.out.println("Times(w1, w2): " + s.times(w1, w2));
		System.out.println("Times(w2, w1): " + s.times(w2, w1));
		System.out.println("Times(w1, zero): " + s.times(w1, zero));
		System.out.println("Times(zero, w1): " + s.times(zero, w1));
		System.out.println("Times(w1, one): " + s.times(w1, one));
		System.out.println("Times(one, w1): " + s.times(one, w1));
		System.out.println("Times(zero, one): " + s.times(zero, one));
		System.out.println("Times(one, zero): " + s.times(one, zero));
		System.out.println("Times(zero, zero): " + s.times(zero, zero));
		System.out.println("Times(one, one): " + s.times(one, one));
		
		System.out.println("Divide(w1, w2): " + s.divide(w1, w2));
		System.out.println("Divide(w2, w1): " + s.divide(w2, w1));
		System.out.println("Divide(w1, zero): " + s.divide(w1, zero));
		System.out.println("Divide(zero, w1): " + s.divide(zero, w1));
		System.out.println("Divide(w1, one): " + s.divide(w1, one));
		System.out.println("Divide(one, w1): " + s.divide(one, w1));
		System.out.println("Divide(zero, one): " + s.divide(zero, one));
		System.out.println("Divide(one, zero): " + s.divide(one, zero));
		System.out.println("Divide(zero, zero): " + s.divide(zero, zero));
		System.out.println("Divide(one, one): " + s.divide(one, one));
	}
}
