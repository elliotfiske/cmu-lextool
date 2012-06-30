import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;

import edu.cmu.sphinx.linguist.dictionary.Word;


public class GenerateFSA {
	
	public static LinkedList<State> states;
	public static HashSet<Word> symbolSet;
	
	public static void writeSymbols(String isymsPath, String osymsPath) {
		int symbolId = 0;
		int stateId = 0;
		FileWriter inputFile = null;
		FileWriter stateFile = null;
		
		
		try {
			inputFile = new FileWriter(isymsPath);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		try {
			stateFile = new FileWriter(osymsPath);
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		BufferedWriter isyms = new BufferedWriter(inputFile);
		BufferedWriter ssyms = new BufferedWriter(stateFile);
		
		try {
			isyms.write("<eps>" + " " + symbolId++ + '\n');
		} catch (IOException e1) {
			e1.printStackTrace();
		}
		
		for (Word s : symbolSet) {
			if (s.toString() != "<eps>") {
				try {
					isyms.write(s.toString() + " " + symbolId++ + '\n');
				} catch (IOException e) {
					e.printStackTrace();
				}
			}
		}
		
		for (State state : states) {
			try {
				if (state.getWords() != null) {
					ssyms.write(state.getWords().toString() + " " + stateId++ + '\n');
				} else {
					ssyms.write(state.toString() + " " + stateId++ + '\n');
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		
		try {
			isyms.close();
			ssyms.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	public static LinkedList<State> concatenateLists(LinkedList<State> list1, LinkedList<State> list2) {
		LinkedList<State> list = new LinkedList<State>();
		
		list.addAll(list1);
		list.addAll(list2);
	
		return list;
	}
	
	public static HashSet<Word> uniteSets(HashSet<Word> set1, HashSet<Word> set2) {
		HashSet<Word> set = new HashSet<Word>();
		set.addAll(set1);
		set.addAll(set2);
		
		return set;		
	}
	
	public static void main (String[] args) throws IOException {
		
		FSA hyperstringFSA = new FSA(args[0], true);
		hyperstringFSA.writeToFile(args[1] + ".fst.txt");
		//hyperstringFSA.writeSymbolsToFile(args[1] + "_isyms", args[1] + "_ssyms");
		
		LanguageModelFSA lmFSA = new LanguageModelFSA(new URL("file:"+ args[2]), 
				new URL("file:models/lm_giga_5k_nvp.sphinx.dic"), 
				new URL("file:models/lm_giga_5k_nvp.sphinx.filler" ));
		
		lmFSA.writeToFile(args[3] + ".fst.txt");
		
		LinkedList<State> hyperstringStates = hyperstringFSA.getStates();
		LinkedList<State> lmStates = lmFSA.getStates();
		
		states = concatenateLists(hyperstringStates, lmStates);
		symbolSet = uniteSets(hyperstringFSA.getSymbols(), lmFSA.getSymbols());
		
		writeSymbols("../testdir/isyms", "../testdir/ssyms");
	}
}
