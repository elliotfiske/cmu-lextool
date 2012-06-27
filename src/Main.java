/*                                                                              
 * 
 * Copyright 1999-2004 Carnegie Mellon University.  
 * Portions Copyright 2004 Sun Microsystems, Inc.  
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */


import java.io.IOException;
import java.net.URL;

public class Main {
	
	public static void main(String[] args) throws IOException, ClassNotFoundException {
	
		// hyperstring FSA
		FSA h = new FSA("This is exciting", true);
		System.out.println(h.toString());
		
		// language model FSA
		LanguageModelFSA lm_fsa = new LanguageModelFSA(new URL("file:"+ args[0]), 
				new URL("file:" + args[1]), new URL("file:" + args[2]));

	}
}
