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
import java.util.*;
import org.apache.commons.lang.WordUtils;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Word;
public class Main {
	
	/**
	 * Generate a map of words with capitalized, lowercased and uppercased forms
	 * @param s
	 * @return a map
	 */
	public static HashMap<String, LinkedList<String>> generateMap(String s) {
		HashMap<String, LinkedList<String>> map = new HashMap<String, LinkedList<String>>();
		for (String word : s.split(" ")) {
			word = word.toLowerCase();
			if (!map.containsKey(word)) {
				map.put(word, new LinkedList<String>());
				map.get(word).add(word);
				map.get(word).add(WordUtils.capitalize(word));
				map.get(word).add(word.toUpperCase());
			}
		}
		return map;
	}
	
	
	public static void main(String[] args) throws IOException, ClassNotFoundException {
		
		// hyperstring FSA
		FSA h = new FSA("This is a sentence", true);
		System.out.println(h.toString());
		
		// language model FSA
		LM_FSA lm_fsa = new LM_FSA(new URL("file:sas_lm"), 
				new URL("file:lm_giga_5k_nvp.sphinx.dic"), new URL("file:lm_giga_5k_nvp.sphinx.filler"));

	}
}
