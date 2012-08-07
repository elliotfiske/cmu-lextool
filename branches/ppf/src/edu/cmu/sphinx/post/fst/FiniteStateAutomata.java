package edu.cmu.sphinx.post.fst;
public interface FiniteStateAutomata {
	
	public void writeToFile(String path);
	
	public void writeSymbolsToFile(String inputSymbolsPath, String outputSymbolsPath);
	
	public String toString();
	
}
