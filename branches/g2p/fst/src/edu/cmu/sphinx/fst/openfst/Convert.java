/**
 * 
 * Copyright 1999-2012 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.fst.openfst;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.fst.SymbolTable;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Convert {
	
	private Convert() {}
	
	public static <T> void export(Fst<T> fst, Semiring<T> semiring, String basename) {
		exportSymbols(fst.getIsyms(), basename+".input.syms");
		exportSymbols(fst.getOsyms(), basename+".output.syms");
		exportFst(fst, semiring, basename+".fst.txt");
	}

	private static <T> void exportFst(Fst<T> fst, Semiring<T> semiring, String filename) {
		FileWriter file;
		try {
			file = new FileWriter(filename);
			PrintWriter out = new PrintWriter(file);
			  
			  
			for(int i=0; i<fst.getNumStates(); i++) {
				State<T> s = fst.getState(i);
				if(!s.getFinalWeight().equals(semiring.zero())) {
					// this is a final state
					out.println(i + "\t" + s.getFinalWeight().getValue());
				}
				
				for(int j=0;j<s.getNumArcs();j++) {
					Arc<T> arc = s.getArc(j);
					String isym = fst.getIsyms().get(arc.getIlabel());
					String osym = fst.getOsyms().get(arc.getOlabel());
					out.println(i + "\t" + arc.getNextState() + "\t" + isym + "\t" + osym + "\t" + arc.getWeight().getValue());
				}
			}
					
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	private static <T> void exportSymbols(SymbolTable syms, String filename) {
		FileWriter file;
		try {
			file = new FileWriter(filename);
			PrintWriter out = new PrintWriter(file);
			  
			  
			for(int i=0; i<syms.size(); i++) {
				String sym = syms.get(i);
				out.println(sym +"\t"+i);
			}
					
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public static Fst<Double> importDouble(String input_file) {
		Fst<Double> fst = new Fst<Double>();
		TropicalSemiring ts = new TropicalSemiring();
		SymbolTable isyms = new SymbolTable();
		SymbolTable osyms = new SymbolTable();
		
		//Add eps, separator, phi, start and end symbols
		isyms.put(0, "<eps>");
		osyms.put(0, "<eps>");
		isyms.put(1, "|");
		osyms.put(1, "|");
		isyms.put(2, "<phi>");
		osyms.put(2, "<phi>");
		isyms.put(3, "<s>");
		osyms.put(3, "<s>");
		isyms.put(4, "</s>");
		osyms.put(4, "</s>");
		
		// Parse input
		System.out.println("Parsing input model...");
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(input_file);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
			System.exit(1);
		}
		
		DataInputStream dis = new DataInputStream(fis);			
		BufferedReader br = new BufferedReader(new InputStreamReader(dis));
		String strLine;		
		int numArcs=0;
		boolean firstLine = true;
		try {
			while ((strLine = br.readLine()) != null) {
				String[] tokens = strLine.split("\\t");
				if (tokens.length > 2) {
				int inputState = Integer.parseInt(tokens[0]);
				int nextState = Integer.parseInt(tokens[1]);
				// Check for states and insert all missing.
				// That way keeps the original state_ids
				int newStates = ((inputState<nextState)?nextState:inputState) - fst.getStates().size() + 1;
				for(int i=0; i<newStates; i++) {
					State<Double> s = new State<Double>(ts.zero());
					fst.addState(s);
				}
				if (firstLine) {
					firstLine = false;
					fst.setStart(inputState);
				}
				
					// Adding arc
					if (isyms.get(tokens[2]) == null) {
						isyms.put(isyms.size(), tokens[2]);
					}
					int iLabel = isyms.get(tokens[2]);
					
					if (osyms.get(tokens[3]) == null) {
						osyms.put(osyms.size(), tokens[3]);
					}
					int oLabel = osyms.get(tokens[3]);
					Weight<Double> arcWeight = new Weight<Double>(Double.parseDouble(tokens[4]));
					Arc<Double> arc = new Arc<Double>(arcWeight, iLabel, oLabel, nextState);
					fst.addArc(inputState, arc);
					numArcs++;
				} else {
					// This is a final State
					int inputState = Integer.parseInt(tokens[0]);
					Weight<Double> finalWeight = new Weight<Double>(Double.parseDouble(tokens[1]));

					fst.setFinal(inputState, finalWeight);
				}
			}
			dis.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		fst.setStart(0);
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
        System.out.println("Import completed.");
        System.out.println("Total States Imported: " + fst.getNumStates());
        System.out.println("Total Arcs Imported: " + numArcs);
        return fst;
	}
}
