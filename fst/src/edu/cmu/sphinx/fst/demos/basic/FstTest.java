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

import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectOutputStream;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * Creates the example wfst depicted in Fig. 1 in paper:
 * 
 * C. Allauzen, M. Riley, J. Schalkwyk, W. Skut, M. Mohri, 
 * "OpenFst: a general and efficient weighted finite-state transducer library", 
 * Proceedings of the 12th International Conference on Implementation and 
 * Application of Automata (CIAA 2007), pp. 11–23, Prague, Czech Republic, 
 * July 2007.
 *  
 * @author "John Salatas <jsalatas@users.sourceforge.net>"
 *
 */
public class FstTest {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		Fst<Double> fst = new Fst<Double>();
		
		// State 0
		State<Double> s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.5), 1, 1, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(1.5), 2, 2, 1));
		fst.AddState(s);
		
		// State 1
		s = new State<Double>();
		s.AddArc(new Arc<Double>(new Weight<Double>(2.5), 3, 3, 2));
		fst.AddState(s);

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(3.5));
		fst.AddState(s);
		
		// Save to disk
		try {
			FileOutputStream fileOut = new FileOutputStream("test.fst");
	        ObjectOutputStream out = new ObjectOutputStream(fileOut);
	        out.writeObject(fst);
	        out.close();
	        fileOut.close();			
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
