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

package edu.cmu.sphinx.fst.openfst;

import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Export {
	private Export() {}
	
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if(args.length < 2) {
			System.err.println("Input and output files not provided");
			System.err.println("You need to provide both the input serialized java fst model");
			System.err.println("and the output binary openfst model.");
			System.exit(1);
		}
		
		@SuppressWarnings("unchecked")
		Fst<Double> fst = (Fst<Double>) Fst.loadModel(args[0]);
		
		// Serialize the java fst model to disk
		System.out.println("Saving as openfst text model...");
        Convert.export(fst, new TropicalSemiring(), args[1]);
	}

}
