/**
 * 
 */
package edu.cmu.sphinx.fst.openfst;

import java.io.IOException;
import java.util.HashMap;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.swig.FstRead;
import edu.cmu.sphinx.fst.swig.ExportedArc;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Import {
	  static {
		    System.loadLibrary("fstreader");
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
		
		Fst<Double> fst = new Fst<Double>();
		
		// Load openfst binary model
		System.out.println("Loading openfst model...");
		FstRead openfst = new FstRead(args[0]);
		if (!openfst.modelLoaded()) {	
			System.exit(1);
		}
		
		// Import input symbols
		System.out.println("Importing input symbols...");
		HashMap<Integer, String> isyms = new HashMap<Integer, String>();
		int numInputSyms = openfst.numInputSyms();
		for (int i=0;i<numInputSyms; i++) {
			isyms.put(i, openfst.inputSym(i));
		}
		fst.setIsyms(isyms);
		
		// Import output symbols
		System.out.println("Importing output symbols...");
		HashMap<Integer, String> osyms = new HashMap<Integer, String>();
		int numOutputSyms = openfst.numOutputSyms();
		for (int i=0;i<numOutputSyms; i++) {
			osyms.put(i, openfst.outputSym(i));
		}
		fst.setOsyms(osyms);
		
		// Import states
		System.out.println("Importing states...");
		int numStates = openfst.numStates();
		for(int i=0; i<numStates; i++) {
			State<Double> s = new State<Double>();
			s.setFinalWeight(new Weight<Double>(openfst.finalWeight(i)));
			fst.AddState(s);
		}
		
		// Import arcs
		System.out.println("Importing arcs...");
		for(int i=0; i<numStates; i++) {
			int numArcs = openfst.numArcs(i);
			for(int j=0; j<numArcs; j++) {
				ExportedArc ea = openfst.getArc(i, j);
				Arc<Double> arc = new Arc<Double>(new Weight<Double>(ea.getWeight()), ea.getIlabel(), ea.getOlabel(), ea.getNextstate());
				fst.addArc(i, arc);
			}
			
		}

		// Serialize the java fst model to disk
		System.out.println("Saving as binary java fst model...");
        try {
        	fst.saveModel(args[1]);
        } catch (IOException e) {
			System.err.println("Cannot write to file " + args[1]);
			System.exit(1); 
		}
        System.out.println("Import completed.");
	}
}
