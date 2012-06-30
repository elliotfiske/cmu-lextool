import java.io.IOException;


public class CreateHyperstringFSA {
	
	
	public static void main (String[] args) throws IOException {
		
		FSA hyperstringFSA = new FSA(args[0], true);
		hyperstringFSA.writeToFile(args[1]);
		hyperstringFSA.writeSymbolsToFile(args[1] + "_isyms", args[1] + "_ssyms");
	}
	
}
