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

package edu.cmu.sphinx.fst.decoder;

import java.util.HashSet;
import java.util.Vector;

import com.google.common.collect.HashBiMap;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.fst.SymbolTable;
import edu.cmu.sphinx.fst.operations.ArcSort;
import edu.cmu.sphinx.fst.operations.Compose;
import edu.cmu.sphinx.fst.operations.ILabelCompare;
import edu.cmu.sphinx.fst.operations.Project;
import edu.cmu.sphinx.fst.operations.ProjectType;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Utils;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Decoder {
    String eps = "<eps>";
    String se = "</s>";
    String sb = "<s>";
    String skip = "_";
    String tie;
    int order = 9;
    float alpha = 0.65f;
    float precision = 0.85f;
    float ratio = 0.72f;
    Vector<Double> thetas = new Vector<Double>();
    boolean mbrdecode = false;
    HashSet<String> skipSeqs = new HashSet<String>();
    HashBiMap<Vector<String>, Integer> clusters = HashBiMap.create();
    //FST stuff
    Fst<Double> g2pmodel;
    Fst<Double> epsMapper;
    SymbolTable isyms;
    SymbolTable osyms;
        

	@SuppressWarnings("unchecked")
	public Decoder(String g2pmodel_file) {
        skipSeqs.add(eps);
        skipSeqs.add(sb);
        skipSeqs.add(se);
        skipSeqs.add(skip);
        skipSeqs.add("-");
        
        g2pmodel = (Fst<Double>) Fst.loadModel(g2pmodel_file);
        isyms = g2pmodel.getIsyms();
        osyms = g2pmodel.getOsyms();
        tie  = isyms.get(1); // The separator symbol is reserved for index 1
        
        loadClusters();
        epsMapper = makeEpsMapper();
        
        ArcSort.apply(g2pmodel, new ILabelCompare<Double>());

    }
    
	public Decoder(String g2pmodel_file, boolean mbrdecode, float alpha, float precision, float ratio, int order) {
    	this(g2pmodel_file);
    	this.mbrdecode = mbrdecode;
    	this.alpha = alpha;
    	this.precision = precision;
    	this.ratio = ratio;
    	this.order = order;
    }

    private void loadClusters() {
		for(int i = 2; i<isyms.size(); i++) {
			String sym = isyms.get(i);
			if(sym.contains(tie)) {
				Vector<String> tmp = Utils.split_string(sym, tie);
				Vector<String> cluster = new Vector<String>();
				for (int j=0;j<tmp.size();j++) {
					if(!tmp.get(j).equals(tie)) {
						cluster.add(tmp.get(j));
					}
				}
				clusters.put(cluster, i - 2);
			}
		}
		
	}

    private Fst<Double> makeEpsMapper() {
		Fst<Double> fst = new Fst<Double>();
		
		TropicalSemiring ts = new TropicalSemiring();
		
		fst.addState(new State<Double>(ts.zero()));
		fst.setStart(0);
		for(int i=0; i<osyms.size(); i++) {
			String sym = osyms.get(i);
			if (skipSeqs.contains(sym)) {
				fst.addArc(0, new Arc<Double>(ts.one(), i, 0, 0)); 
			} else {
				fst.addArc(0, new Arc<Double>(ts.one(), i, i, 0)); 
			}
		}
		
		fst.setFinal(0, ts.one());
		ArcSort.apply(fst, new ILabelCompare<Double>());
		fst.setIsyms(osyms);
		fst.setOsyms(osyms);
		
    	return fst;
	}
    
    public Vector<Path> phoneticize(Vector<String> entry, int nbest, int beam) {
    	Fst<Double> result;
    	Fst<Double> epsMapped;
    	Fst<Double> shortest;
    	Fst<Double> efst = entryToFSA(entry);
    	Fst<Double> smbr;
        int N = compute_thetas(entry.size());
        result = Compose.apply(efst, g2pmodel, new TropicalSemiring());
        Project.apply(result, ProjectType.OUTPUT);
        // TODO: Not yet completed
/*
        if( nbest > 1 ){
            //This is a cheesy hack. 
            shortest = ShortestPath.apply(result, beam);
        }else{
        	shortest = ShortestPath.apply(result, shortest, 1);
        }
        
        shortest = RmEpsilon.apply(shortest);
        FstPathFinder pathfinder = new FstPathFinder(skipSeqs);
        pathfinder.findAllStrings(shortest);
        
        return pathfinder.paths;
*/
        return null;
    }
    
	/**
     * Theta values are computed on a per-word basis
     * We scale the maximum order by the length of the input word.
     * Higher MBR N-gram orders favor longer pronunciation hypotheses.
     * Thus a high N-gram order coupled with a short word will
     * favor longer pronunciations with more insertions.
     * 
     *   p=.63, r=.48
     *   p=.85, r=.72
     * .918
     * Compute the N-gram Theta factors for the
     * model.  These are a function of,
     *   N:  The maximum N-gram order
     *   T:  The total number of 1-gram tokens 
     *   p:  The 1-gram precision
     *   r:  A constant ratio
     *        
     * 1) T may be selected arbitrarily.
     * 2) Default values are selected from Tromble 2008
     * @param size
     * @return
     */
    private int compute_thetas(int size) {
    	thetas.removeAllElements();
    	
    	double t = 10.0;
    	int n = Math.min(size+1, order);
    	// usuned. commenting it out.
    	//float ip = -0.3; 
    	thetas.add(-1/t);
    	for(int i=1; i<=order; i++) {
    	      thetas.add(1.0/((i*t*precision) * (Math.pow(ratio,(i-1)))));
    	}
    	return n;
	}

	/**
     * Transforms an input spelling/pronunciation into an equivalent
     * FSA, adding extra arcs as needed to accomodate clusters.
     * 
     * @param entry
     * @return
     */
    private Fst<Double> entryToFSA(Vector<String> entry ){
    	Fst<Double> efst = new Fst<Double>();
    	
    	State<Double> s = new State<Double>(0.);
    	efst.addState(s);
    	efst.addArc(0, new Arc<Double>(new Weight<Double>(0.), isyms.get(sb), osyms.get(sb), 1));
    	
        //Build the basic FSA
    	int i;
    	for (i=0; i< entry.size(); i++) {
			String str = entry.get(i);
			s = new State<Double>(0.);
	    	efst.addState(s);
	    	efst.addArc(i+1, new Arc<Double>(new Weight<Double>(0.), isyms.get(str), osyms.get(str), i+2));
		}
    	
    	//Add any cluster arcs
        for(int j=0; j<clusters.size(); j++) {
    		int start = 0;
    		int k = 0;
    		Vector<String> cluster = clusters.inverse().get(j);
    		while(k < entry.size()) {
    			k = Utils.search(entry, cluster, start, entry.size(), 0, cluster.size());
    			if (k != entry.size()) {
    				efst.addArc(k+1, new Arc<Double>(new Weight<Double>(0.), 
    						clusters.get(cluster), 
    						clusters.get(cluster), k+cluster.size()+1));
    				start++;
    			}
    			
    		}
    	}
    	
        efst.addState(new State<Double>(new Weight<Double>(0.)));
        efst.addArc( i+1, new Arc<Double>(new Weight<Double>(0.), isyms.get(se), isyms.get(se), i+2));
        efst.setFinal(i+2, new Weight<Double>(0.));
        efst.setIsyms(isyms);
        efst.setOsyms(isyms);

    	
    	return efst;
    }
}
