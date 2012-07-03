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
import com.google.common.collect.HashBiMap;

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
	private Import() {}
	
	public static Fst<Double> convert(String input_file) {
		Fst<Double> fst = new Fst<Double>();
		TropicalSemiring ts = new TropicalSemiring();
		HashBiMap<Integer, String> isyms = HashBiMap.create();
		HashBiMap<Integer, String> osyms = HashBiMap.create();
		
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
					State<Double> s = new State<Double>();
					s.setFinalWeight(ts.zero());
					fst.AddState(s);
				}
				
					// Adding arc
					if (isyms.inverse().get(tokens[2]) == null) {
						isyms.put(isyms.size(), tokens[2]);
					}
					int iLabel = isyms.inverse().get(tokens[2]);
					
					if (osyms.inverse().get(tokens[3]) == null) {
						osyms.put(osyms.size(), tokens[3]);
					}
					int oLabel = osyms.inverse().get(tokens[3]);
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
		
		Fst<Double> fst = Import.convert(args[0]);
		
		// Serialize the java fst model to disk
		System.out.println("Saving as binary java fst model...");
        try {
        	fst.saveModel(args[1]);
        } catch (IOException e) {
			System.err.println("Cannot write to file " + args[1]);
			System.exit(1); 
		}
		
	}
}
