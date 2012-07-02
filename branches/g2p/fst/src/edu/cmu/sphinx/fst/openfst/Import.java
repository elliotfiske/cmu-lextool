/**
 * 
 */
package edu.cmu.sphinx.fst.openfst;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.HashMap;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Import {

	/**
	 * @param args
	 */
	public static void main(String[] args) {
		if(args.length < 2) {
			System.err.println("Input and output files not provided");
			System.err.println("You need to provide both the input binary openfst model");
			System.err.println("and the output serialized java fst model.");
			System.exit(1);
		}
		
		Fst<Double> fst = new Fst<Double>();
		TropicalSemiring ts = new TropicalSemiring();
		HashMap<String, Integer> isyms = new HashMap<String, Integer>();
		HashMap<String, Integer> osyms = new HashMap<String, Integer>();
		
		//Add eps, separator, phi, start and end symbols
		isyms.put("<eps>", 0);
		osyms.put("<eps>", 0);
		isyms.put("}", 1);
		osyms.put("}", 1);
		isyms.put("<phi>", 2);
		osyms.put("<phi>", 2);
		isyms.put("<s>", 3);
		osyms.put("<s>", 3);
		isyms.put("</s>", 4);
		osyms.put("</s>", 4);
		
		// Parse input
		System.out.println("Parsing input model...");
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(args[0]);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
			System.exit(1);
		}
		
		DataInputStream dis = new DataInputStream(fis);			
		BufferedReader br = new BufferedReader(new InputStreamReader(dis));
		String strLine;		
		int numArcs=0;
		try {
			while ((strLine = br.readLine()) != null) {
				String[] tokens = strLine.split("\\t");
				int inputState = Integer.parseInt(tokens[0]);
				int nextState = Integer.parseInt(tokens[1]);
				// Check for states and insert all missing.
				// That way keeps the original state_ids
				int newStates = ((inputState<nextState)?nextState:inputState) - fst.getStates().size() + 1;
				for(int i=0; i<newStates; i++) {
					State<Double> s = new State<Double>();
					s.setFinalWeight(ts.one());
					fst.AddState(s);
				}
				
				if (tokens.length > 2) {
					// Adding arc
					if (isyms.get(tokens[2]) == null) {
						isyms.put(tokens[2], isyms.size());
					}
					int iLabel = isyms.get(tokens[2]);
					
					if (osyms.get(tokens[3]) == null) {
						osyms.put(tokens[3], osyms.size());
					}
					int oLabel = osyms.get(tokens[3]);
					Weight<Double> arcWeight = new Weight<Double>(Double.parseDouble(tokens[4]));
					Arc<Double> arc = new Arc<Double>(arcWeight, iLabel, oLabel, nextState);
					fst.addArc(inputState, arc);
					numArcs++;
				} else {
					// This is a final State
					fst.setFinal(inputState, ts.zero());
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
		
		// Serialize the java fst model to disk
		System.out.println("Saving as binary java fst model...");
        try {
        	fst.saveModel(args[1]);
        } catch (IOException e) {
			System.err.println("Cannot write to file " + args[1]);
			System.exit(1); 
		}
        System.out.println("Import completed.");
        System.out.println("Total States Imported: " + fst.getNumStates());
        System.out.println("Total Arcs Imported: " + numArcs);
	}
}
