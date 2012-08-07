package edu.cmu.sphinx.post.fst;
import java.io.IOException;
import java.net.URL;


public class CreateLmFSA {
	public static void main (String[] args) throws IOException {
		
		String lm_path = null, output_path = null;
		
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("-lm")) lm_path = args[i+1];
			else if (args[i].equals("-path")) output_path = args[i+1];
		}
		
		if (args.length < 4 || lm_path == null || output_path == null) {
			System.out.println("Usage: sh ./lm2fst.sh -lm lm_path -path output_path ");
			return;
		}
		
		LanguageModelFSA lmFSA = new LanguageModelFSA(new URL("file:"+ lm_path), 
				new URL("file:../models/lm_giga_5k_nvp.sphinx.dic"), 
				new URL("file:../models/lm_giga_5k_nvp.sphinx.filler" ));
		
		lmFSA.writeToFile(output_path + ".fst.txt");

		lmFSA.writeSymbolsToFile(output_path + ".isyms", output_path+ ".ssyms");
		
	}
}
