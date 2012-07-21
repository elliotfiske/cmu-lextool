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

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Vector;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.operations.ArcSort;
import edu.cmu.sphinx.fst.operations.Compose;
import edu.cmu.sphinx.fst.operations.ILabelCompare;
import edu.cmu.sphinx.fst.operations.NShortestPaths;
import edu.cmu.sphinx.fst.operations.Project;
import edu.cmu.sphinx.fst.operations.ProjectType;
import edu.cmu.sphinx.fst.operations.RmEpsilon;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Utils;
import edu.cmu.sphinx.fst.weight.Semiring;
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

    Vector<Double> thetas = new Vector<Double>();
    HashSet<String> skipSeqs = new HashSet<String>();
    Mapper<Vector<String>, Integer> clusters = new Mapper<Vector<String>, Integer>();
    //FST stuff
    Fst<Double> g2pmodel;
    Fst<Double> g2pmodel_copy;
    Mapper<Integer, String> isyms;
    Mapper<Integer, String> osyms;
    boolean persistModel = false;

	public Decoder(String g2pmodel_file, boolean persistModel) {
        skipSeqs.add(eps);
        skipSeqs.add(sb);
        skipSeqs.add(se);
        skipSeqs.add(skip);
        skipSeqs.add("-");
        this.persistModel = persistModel;
        
        g2pmodel = Fst.loadModel(g2pmodel_file);
        
        isyms = g2pmodel.getIsyms();
        osyms = g2pmodel.getOsyms();
        tie  = isyms.getValue(1); // The separator symbol is reserved for index 1
        
        loadClusters();

        ArcSort.apply(g2pmodel, new ILabelCompare<Double>());
        
        if (this.persistModel) {
        	// keep a copy 
        	g2pmodel_copy = g2pmodel.copy();
        }
    }
    
	
	
	/**
	 * @return the isyms
	 */
	public Mapper<Integer, String> getIsyms() {
		return isyms;
	}

	/**
	 * 
	 */
    private void loadClusters() {
		for(int i = 2; i<isyms.size(); i++) {
			String sym = isyms.getValue(i);
			if(sym.contains(tie)) {
				Vector<String> tmp = Utils.split_string(sym, tie);
				Vector<String> cluster = new Vector<String>();
				for (int j=0;j<tmp.size();j++) {
					if(!tmp.get(j).equals(tie)) {
						cluster.add(tmp.get(j));
					}
				}
				clusters.put(cluster, i);
			}
		}
		
	}

    /**
     * 
     * @param entry
     * @param nbest
     * @return
     */
    public ArrayList<Path<Double>> phoneticize(Vector<String> entry, int nbest) {
    	if(persistModel) {
    		g2pmodel = g2pmodel_copy.copy();
    	}
    	Fst<Double> efst = entryToFSA(entry);
        
    	Fst<Double> result = Compose.get(efst, g2pmodel, new TropicalSemiring());
        
        Project.apply(result, ProjectType.OUTPUT);
    	
    	Fst<Double> shortest = NShortestPaths.get(result, nbest, false);

    	shortest = RmEpsilon.get(shortest);

    	ArrayList<Path<Double>> paths = Decoder.findAllPaths(shortest, skipSeqs, tie);
        
		return paths;
    }
    
	/**
     * Transforms an input spelling/pronunciation into an equivalent
     * FSA, adding extra arcs as needed to accomodate clusters.
     * 
     * @param entry the input vector
     * @return the created fst
     */
    private Fst<Double> entryToFSA(Vector<String> entry ){
    	TropicalSemiring ts = new TropicalSemiring();
    	Fst<Double> efst = new Fst<Double>(ts);
    	
    	State<Double> s = new State<Double>(ts.zero());
    	efst.addState(s);
    	efst.addArc(s.getId(), new Arc<Double>(isyms.getKey(sb), isyms.getKey(sb), 0., "1"));
    	efst.setStart(s.getId());
    	
        //Build the basic FSA
    	int i;
    	for (i=0; i< entry.size(); i++) {
			String str = entry.get(i);
			s = new State<Double>(ts.zero());
	    	efst.addState(s);
	    	efst.addArc(s.getId(), new Arc<Double>(isyms.getKey(str), isyms.getKey(str), 0., Integer.toString(i+2)));
		}
    	
    	//Add any cluster arcs
    	for(Iterator<Vector<String>> it = clusters.keySet().iterator(); it.hasNext();) {
    		int start = 0;
    		int k = 0;
    		Vector<String> cluster = it.next();
    		while(k != -1) {
    			k = Utils.search(entry, cluster, start);
    			if (k != -1) {
    				efst.addArc(Integer.toString(start+k+1), new Arc<Double>(clusters.getValue(cluster),clusters.getValue(cluster), 0., Integer.toString(start+k+cluster.size()+1)));
    				start = start + k + cluster.size();
    			}
    			
    		}
    	}
    	
        efst.addState(new State<Double>(ts.zero()));
        efst.addState(new State<Double>(ts.zero()));
        efst.addArc(Integer.toString(i+1), new Arc<Double>(isyms.getKey(se), isyms.getKey(se), 0., Integer.toString(i+2)));
        efst.setFinal(Integer.toString(i+2), new Weight<Double>(0.));
        efst.setIsyms(isyms);
        efst.setOsyms(isyms);

    	
    	return efst;
    }
    
        /**
     * 
     * @param fst
     * @return
     */
    @SuppressWarnings("unchecked")
	public static ArrayList<Path<Double>> findAllPaths(Fst<Double> fst, HashSet<String> skipSeqs, String tie) {
    	Semiring<Double> semiring =fst.getSemiring();
    	    	
    	ArrayList<Path<Double>> finalPaths = new ArrayList<Path<Double>>();
    	HashMap<State<Double>, Path<Double>> paths = new HashMap<State<Double>, Path<Double>>();
    	ArrayList<State<Double>> queue = new ArrayList<State<Double>>();
    	Path<Double> p = new Path<Double>(fst.getSemiring());
		p.setCost(semiring.one());
		paths.put(fst.getStart(), p);

    	queue.add(fst.getStart());
    	
    	while(queue.size()>0) {
    		State<Double> s = queue.get(0);
    		queue.remove(0);
    		if(!s.getFinalWeight().equals(semiring.zero())) {
    			finalPaths.add(paths.get(s));
    		}
    		
    		for(int i=0; i<s.getNumArcs(); i++) {
        		Arc<Double> a = s.getArc(i);

        		p = new Path<Double>(fst.getSemiring());
        		Path<Double> cur = paths.get(s);
        		p.setCost(cur.getCost());
        		p.setPath((ArrayList<String>) cur.getPath().clone());

        		String sym = fst.getOsyms().getValue(a.getOlabel());
        		sym = sym.replace(tie, " ");
        		if(!skipSeqs.contains(sym)) {
        			p.getPath().add(sym);
        		}
        		p.setCost(semiring.times(p.getCost(), a.getWeight()));
        		State<Double> nextState = fst.getStateById(a.getNextStateId());
        		paths.put(nextState, p);
        		if(!queue.contains(nextState)) {
        			queue.add(nextState);
        		}
    		}
    	}
    	
    	Collections.sort(finalPaths, new PathComparator<Double>());
    	
    	return finalPaths;
    }
}
