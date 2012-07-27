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
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Convert {
	
	private Convert() {}
	
	public static <T extends Comparable<T>> void export(Fst<T> fst, Semiring<T> semiring, String basename) {
		exportSymbols(fst.getIsyms(), basename+".input.syms");
		exportSymbols(fst.getOsyms(), basename+".output.syms");
		exportSymbols(fst.getSsyms(), basename+".states.syms");
		exportFst(fst, semiring, basename+".fst.txt");
	}

	private static <T extends Comparable<T>> void exportFst(Fst<T> fst, Semiring<T> semiring, String filename) {
		FileWriter file;
		try {
			file = new FileWriter(filename);
			PrintWriter out = new PrintWriter(file);
			
			//print start first 
			State<T> s = fst.getStart();
			out.println(s.getId() + "\t" + s.getFinalWeight().getValue());
			
			//print all states 
			for(Iterator<State<T>> itS = fst.stateIterator(); itS.hasNext();) {
				s = itS.next();
				if(!s.getId().equals(fst.getStartId())) {
					out.println(s.getId() + "\t" + s.getFinalWeight().getValue());
				}
			}
			Arc<T> arc;
			String isym;
			String osym;
			for(Iterator<State<T>> itS = fst.stateIterator(); itS.hasNext();) {
				s = itS.next();
				for(Iterator<Arc<T>> itA = s.arcIterator();itA.hasNext();) {
					arc = itA.next();
					isym = (fst.getIsyms()!=null)?fst.getIsyms().getValue(arc.getIlabel()):Integer.toString(arc.getIlabel());
					osym = (fst.getOsyms()!=null)?fst.getOsyms().getValue(arc.getOlabel()):Integer.toString(arc.getOlabel());
					
					out.println(s.getId() + "\t" + arc.getNextStateId() + "\t" + isym + "\t" + osym + "\t" + arc.getWeight().getValue());
				}
			}
					
			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	
	}
	
	private static <T extends Comparable<T>> void exportSymbols(Mapper<Integer, String> syms, String filename) {
		if(syms == null)
			return;
		
		FileWriter file;
		try {
			file = new FileWriter(filename);
			PrintWriter out = new PrintWriter(file);

			for(int i=0; i<syms.size(); i++) {
				String sym = syms.getValue(i);
				out.println(sym +"\t"+i);
			}

			out.close();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	private static Mapper<Integer, String> importSymbols(String filename) {
		Mapper<Integer, String> syms = null;
		
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(filename);
			DataInputStream dis = new DataInputStream(fis);			
			BufferedReader br = new BufferedReader(new InputStreamReader(dis));
			String strLine;		
			syms = new Mapper<Integer, String>();
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
	
	public static Fst<Double> importDouble(String basename, Semiring<Double> semiring) {
		//TropicalSemiring ts = new TropicalSemiring();
		Fst<Double> fst = new Fst<Double>(semiring);
		
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
			for(int i=0; i<keys.size();i++) {
				Integer key = keys.get(i);
				String stateId = ssyms.getValue(key);
				State<Double> s = new State<Double>(semiring.zero());
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
		String strLine;		
		boolean firstLine = true;
		try {
			while ((strLine = br.readLine()) != null) {
				String[] tokens = strLine.split("\\t");
				String inputStateId = tokens[0]; 
				State<Double> inputState = fst.getStateById(inputStateId);
				if(inputState == null) {
					inputState = new State<Double>(semiring.zero());
					inputState.setId(inputStateId);
					inputStateId = fst.addState(inputState);
				}
				
				if (firstLine) {
					firstLine = false;
					fst.setStart(inputStateId);
				}
				
				if (tokens.length > 2) {
					String nextStateId = tokens[1]; 
					

					State<Double> nextState = fst.getStateById(nextStateId);
					if(nextState == null) {
						nextState = new State<Double>(semiring.zero());
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
					Weight<Double> arcWeight = new Weight<Double>(Double.parseDouble(tokens[4]));
					Arc<Double> arc = new Arc<Double>(iLabel, oLabel, arcWeight, nextStateId);
					fst.addArc(inputStateId, arc);
				} else {
					// This is a final weight
					Weight<Double> finalWeight = new Weight<Double>(Double.parseDouble(tokens[1]));
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
