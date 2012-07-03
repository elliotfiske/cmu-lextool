/**
 * 
 */
package edu.cmu.sphinx.fst.decoder;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Vector;

import com.google.common.collect.HashBiMap;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.operations.ILabelCompare;
import edu.cmu.sphinx.fst.operations.Operations;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Utils;
import edu.cmu.sphinx.fst.weight.TropicalSemiring;

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
    Vector<Float> thetas;
    boolean mbrdecode = false;
    HashSet<String> skipSeqs = new HashSet<String>();
    HashMap<Vector<String>, Integer> clusters = new HashMap<Vector<String>, Integer>();
    //FST stuff
    Fst<Double> g2pmodel;
    Fst<Double> epsMapper;
    HashBiMap<Integer, String> isyms;
    HashBiMap<Integer, String> osyms;
        

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
        
        Operations.ArcSort(g2pmodel, new ILabelCompare<Double>());

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
		
		fst.AddState(new State<Double>(ts.zero()));
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
		Operations.ArcSort(fst, new ILabelCompare<Double>());
		fst.setIsyms(osyms);
		fst.setOsyms(osyms);
		
    	return fst;
	}
    
    public void phoneticize() {
    	
    }
}
