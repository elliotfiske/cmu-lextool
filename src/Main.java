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

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.*;
import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Word;
public class Main {
	
	public static FileWriter FSA_output;
	public static BufferedWriter out;
	
	
	public static void main(String[] args) throws IOException, ClassNotFoundException {
		

		FSA_output = new FileWriter("fsa_output");
		out = new BufferedWriter(FSA_output);
		
		// hyperstring FSA
		FSA h = new FSA("This is a sentence", true);
		System.out.println(h.toString());
		
		// language model FSA
		LanguageModelFSA lm_fsa = new LanguageModelFSA(new URL("file:text.arpa"), 
				new URL("file:lm_giga_5k_nvp.sphinx.dic"), new URL("file:lm_giga_5k_nvp.sphinx.filler"));
		
		out.close();
	}
}
