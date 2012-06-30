import java.io.IOException;
import java.net.URL;


public class CreateLmFSA {
	public static void main (String[] args) throws IOException {
		
		LanguageModelFSA lmFSA = new LanguageModelFSA(new URL("file:"+ args[0]), 
				new URL("file:../models/lm_giga_5k_nvp.sphinx.dic"), 
				new URL("file:../models/lm_giga_5k_nvp.sphinx.filler" ));
		
		lmFSA.writeToFile(args[1]);

		lmFSA.writeSymbolsToFile(args[1] + "_isyms", args[1] + "_ssyms");
		
	}
}
