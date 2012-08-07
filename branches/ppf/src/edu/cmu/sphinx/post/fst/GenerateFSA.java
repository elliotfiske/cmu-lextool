package edu.cmu.sphinx.post.fst;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.net.URL;
import java.util.HashSet;
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
				
		String text = null, lm_path = null, output_path = null;
		
		for (int i = 0; i < args.length; i++) {
			if (args[i].equals("-text")) text = args[i+1];
			else if (args[i].equals("-path")) output_path = args[i+1];
			else if (args[i].equals("-lm")) lm_path = args[i+1];
		}
		
		if (args.length < 6 || text == null || output_path == null || lm_path == null) {
			System.out.println("Usage: sh ./generateFST.sh -text t -lm lm_path -path output_path");
			return;
		}
		
		FSA hyperstringFSA = new FSA(text, true);
		hyperstringFSA.writeToFile(output_path + "/hypestring.fst.txt");
		
		LanguageModelFSA lmFSA = new LanguageModelFSA(new URL("file:"+ lm_path), 
				new URL("file:../models/lm_giga_5k_nvp.sphinx.dic"), 
				new URL("file:../models/lm_giga_5k_nvp.sphinx.filler" ));
		
		lmFSA.writeToFile(output_path + "/lm.fst.txt");
		
		LinkedList<State> hyperstringStates = hyperstringFSA.getStates();
		LinkedList<State> lmStates = lmFSA.getStates();
		
		states = concatenateLists(hyperstringStates, lmStates);
		symbolSet = uniteSets(hyperstringFSA.getSymbols(), lmFSA.getSymbols());
		
		writeSymbols(output_path + "/isyms", output_path + "/ssyms");
	}
}
