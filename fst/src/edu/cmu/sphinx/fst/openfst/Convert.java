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
import java.util.ArrayList;
import java.util.Collections;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.utils.Mapper;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Convert {
	
	private Convert() {}
	
	public static void export(Fst fst, Semiring semiring, String basename) {
		exportSymbols(fst.getIsyms(), basename+".input.syms");
		exportSymbols(fst.getOsyms(), basename+".output.syms");
		exportSymbols(fst.getSsyms(), basename+".states.syms");
		exportFst(fst, semiring, basename+".fst.txt");
	}

	private static void exportFst(Fst fst, Semiring semiring, String filename) {
		FileWriter file;
		try {
			file = new FileWriter(filename);
			PrintWriter out = new PrintWriter(file);
			
			//print start first 
			State start = fst.getStart();
			out.println(start.getId() + "\t" + start.getFinalWeight());
			
			//print all states 
			for(State s : fst.getStates()) {
				if(!s.getId().equals(fst.getStartId())) {
					out.println(s.getId() + "\t" + s.getFinalWeight());
				}
			}

			for(State s : fst.getStates()) {
				for(Arc arc: s.getArcs()) {
					String isym = (fst.getIsyms()!=null)?fst.getIsyms().getValue(arc.getIlabel()):Integer.toString(arc.getIlabel());
					String osym = (fst.getOsyms()!=null)?fst.getOsyms().getValue(arc.getOlabel()):Integer.toString(arc.getOlabel());
					
					out.println(s.getId() + "\t" + arc.getNextStateId() + "\t" + isym + "\t" + osym + "\t" + arc.getWeight());
				}
			}
					
			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	
	}
	
	private static void exportSymbols(Mapper<Integer, String> syms, String filename) {
		if(syms == null)
			return;
		
		try {
			FileWriter file = new FileWriter(filename);
			PrintWriter out = new PrintWriter(file);

			for(int i=0; i<syms.size(); i++) {
				String sym = syms.getValue(i);
				out.println(sym +"\t"+i);
			}

			out.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	private static Mapper<Integer, String> importSymbols(String filename) {
		Mapper<Integer, String> syms = null;
		
		try {
			FileInputStream fis = new FileInputStream(filename);
			DataInputStream dis = new DataInputStream(fis);			
			BufferedReader br = new BufferedReader(new InputStreamReader(dis));
			syms = new Mapper<Integer, String>();
			String strLine;		
			while ((strLine = br.readLine()) != null) {
				String[] tokens = strLine.split("\\t");
				String sym = tokens[0];
				Integer index = Integer.parseInt(tokens[1]);
				syms.put(index, sym);
			}
			
		} catch (IOException e1) {
			return null;
		}		
		
		return syms;
	}
	
	public static Fst importFloat(String basename, Semiring semiring) {
		//TropicalSemiring ts = new TropicalSemiring();
		Fst fst = new Fst(semiring);
		
		Mapper<Integer, String> isyms = importSymbols(basename+".input.syms");
		if(isyms == null) {
			isyms = new Mapper<Integer, String>();
			isyms.put(0, "<eps>");
		}
		fst.setIsyms(isyms);
		
		Mapper<Integer, String> osyms = importSymbols(basename+".output.syms");
		if(osyms == null) {
			osyms = new Mapper<Integer, String>();
			osyms.put(0, "<eps>");
		}
		fst.setOsyms(osyms);
		
		Mapper<Integer, String> ssyms = importSymbols(basename+".states.syms");
		if(ssyms != null) {
			ArrayList<Integer> keys = new ArrayList<Integer>(ssyms.keySet());
			Collections.sort(keys);

			// create states according to the ssyms order
			for(Integer key: keys) {
				String stateId = ssyms.getValue(key);
				State s = new State(semiring.zero());
				s.setId(stateId);
				fst.addState(s);
				if(key == 0) {
					fst.setStart(stateId);
				}
			}
		}

		
		// Parse input
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(basename+".fst.txt");
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
			System.exit(1);
		}
		
		DataInputStream dis = new DataInputStream(fis);			
		BufferedReader br = new BufferedReader(new InputStreamReader(dis));
		boolean firstLine = true;
		String strLine;		
		try {
			while ((strLine = br.readLine()) != null) {
				String[] tokens = strLine.split("\\t");
				String inputStateId = tokens[0]; 
				State inputState = fst.getStateById(inputStateId);
				if(inputState == null) {
					inputState = new State(semiring.zero());
					inputState.setId(inputStateId);
					inputStateId = fst.addState(inputState);
				}
				
				if (firstLine) {
					firstLine = false;
					fst.setStart(inputStateId);
				}
				
				if (tokens.length > 2) {
					String nextStateId = tokens[1]; 
					

					State nextState = fst.getStateById(nextStateId);
					if(nextState == null) {
						nextState = new State(semiring.zero());
						nextState.setId(nextStateId);
						nextStateId = fst.addState(nextState);
					}
					// Adding arc
					if (isyms.getKey(tokens[2]) == null) {
						isyms.put(isyms.size(), tokens[2]);
					}
					int iLabel = isyms.getKey(tokens[2]);
					
					if (osyms.getKey(tokens[3]) == null) {
						osyms.put(osyms.size(), tokens[3]);
					}
					int oLabel = osyms.getKey(tokens[3]);
					float arcWeight = Float.parseFloat(tokens[4]);
					Arc arc = new Arc(iLabel, oLabel, arcWeight, nextStateId);
					fst.addArc(inputStateId, arc);
				} else {
					// This is a final weight
					float finalWeight = Float.parseFloat(tokens[1]);
					fst.setFinal(inputStateId, finalWeight);
				}
			}
			dis.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
		fst.setIsyms(isyms);
		fst.setOsyms(osyms);
        return fst;
	}
}
