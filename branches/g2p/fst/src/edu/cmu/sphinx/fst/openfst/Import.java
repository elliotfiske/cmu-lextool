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

import java.io.IOException;

import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Import {
	
	private Import() {}
	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if(args.length < 2) {
			System.err.println("Input and output files not provided");
			System.err.println("You need to provide both the input binary openfst model");
			System.err.println("and the output serialized java fst model.");
			System.exit(1);
		}
		
		Fst<Double> fst = Convert.importDouble(args[0], new TropicalSemiring());
		
		// Serialize the java fst model to disk
		System.out.println("Saving as binary java fst model...");
        try {
        	fst.saveModel(args[1]);
        	
        } catch (IOException e) {
			System.err.println("Cannot write to file " + args[1]);
			System.exit(1); 
		}
	}
}
