package edu.cmu.sphinx.post.fst;
import java.io.IOException;


public class CreateHyperstringFSA {
	
	
	public static void main (String[] args) throws IOException {
		
		String text = null, output_path = null;
		
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("-text")) text = args[i+1];
			else if (args[i].equals("-path")) output_path = args[i+1];
		}
		
		if (args.length < 4 || text == null || output_path == null) {
			System.out.println("Usage: sh ./hyperstring2fst.sh -text t -path output_path ");
			return;
		}
		
		
		FSA hyperstringFSA = new FSA(text, true);
		hyperstringFSA.writeToFile(output_path);
		hyperstringFSA.writeSymbolsToFile(output_path + ".isyms", output_path + ".ssyms");
		
	}
	
}
