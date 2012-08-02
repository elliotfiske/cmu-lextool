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
import java.util.Vector;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.operations.ArcSort;
import edu.cmu.sphinx.fst.operations.Compose;
import edu.cmu.sphinx.fst.operations.ILabelCompare;
import edu.cmu.sphinx.fst.operations.NShortestPaths;
import edu.cmu.sphinx.fst.operations.OLabelCompare;
import edu.cmu.sphinx.fst.operations.Project;
import edu.cmu.sphinx.fst.operations.ProjectType;
import edu.cmu.sphinx.fst.operations.RmEpsilon;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.semiring.TropicalSemiring;
import edu.cmu.sphinx.fst.utils.Utils;

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

    HashSet<String> skipSeqs = new HashSet<String>();
    HashMap<Vector<String>, Integer> clusters = new HashMap<Vector<String>, Integer>();

    Fst g2pmodel;
    Fst epsilonFilter;
    

    public Decoder(String g2pmodel_file) {
        skipSeqs.add(eps);
        skipSeqs.add(sb);
        skipSeqs.add(se);
        skipSeqs.add(skip);
        skipSeqs.add("-");

        g2pmodel = Fst.loadModel(g2pmodel_file);
        // keep an augmented copy (for compose)
        Compose.augment(0, g2pmodel, g2pmodel.getSemiring());
        ArcSort.apply(g2pmodel, new ILabelCompare());

        HashMap<Integer, String> reversedIsyms = Utils.reverseHashMap(g2pmodel.getIsyms());
        tie = reversedIsyms.get(1); // The separator symbol is reserved for index 1

        loadClusters(reversedIsyms);

        // get epsilon filter for composition
        epsilonFilter = Compose.getFilter(g2pmodel.getIsyms(), g2pmodel.getSemiring());
        ArcSort.apply(epsilonFilter, new ILabelCompare());
    }

    /**
     * @return the isyms
     */
    public HashMap<String, Integer> getModelIsyms() {
        return g2pmodel.getIsyms();
    }

    /**
	 * 
	 */
    private void loadClusters(HashMap<Integer, String> reversedIsyms) {
        for (int i = 2; i < reversedIsyms.size(); i++) {
            String sym = reversedIsyms.get(i);
            if (sym.contains(tie)) {
                Vector<String> tmp = Utils.split_string(sym, tie);
                Vector<String> cluster = new Vector<String>();
                for (int j = 0; j < tmp.size(); j++) {
                    if (!tmp.get(j).equals(tie)) {
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
    public ArrayList<Path> phoneticize(Vector<String> entry, int nbest) {
        Fst efst = entryToFSA(entry);
        Semiring s = efst.getSemiring();
        Compose.augment(1, efst, s);
        ArcSort.apply(efst, new OLabelCompare());
        Fst result = Compose.compose(efst, epsilonFilter, s, true);
        ArcSort.apply(result, new OLabelCompare());
        result = Compose.compose(result, g2pmodel, s, true);
        Project.apply(result, ProjectType.OUTPUT);
        result = NShortestPaths.get(result, nbest, false);
        result = RmEpsilon.get(result);
        ArrayList<Path> paths = Decoder.findAllPaths(result, skipSeqs, tie);
        result = null;

        return paths;
    }

    /**
     * Transforms an input spelling/pronunciation into an equivalent FSA, adding
     * extra arcs as needed to accomodate clusters.
     * 
     * @param entry the input vector
     * @return the created fst
     */
    private Fst entryToFSA(Vector<String> entry) {
        TropicalSemiring ts = new TropicalSemiring();
        Fst efst = new Fst(ts);

        State s = new State(ts.zero());
        efst.addState(s);
        efst.addArc(s.getId(), new Arc(g2pmodel.getIsyms().get(sb), g2pmodel.getIsyms().get(sb), 0.f,
                1));
        efst.setStart(s.getId());

        // Build the basic FSA
        int i;
        for (i = 0; i < entry.size(); i++) {
            String str = entry.get(i);
            s = new State(ts.zero());
            efst.addState(s);
            efst.addArc(s.getId(), new Arc(g2pmodel.getIsyms().get(str),
                    g2pmodel.getIsyms().get(str), 0.f, i + 2));
        }

        // Add any cluster arcs
        for (Vector<String> cluster : clusters.keySet()) {
            Integer value = clusters.get(cluster);
            int start = 0;
            int k = 0;
            while (k != -1) {
                k = Utils.search(entry, cluster, start);
                if (k != -1) {
                    efst.addArc(start + k + 1, new Arc(value, value, 0.f, start
                            + k + cluster.size() + 1));
                    start = start + k + cluster.size();
                }
            }
        }

        efst.addState(new State(ts.zero()));
        efst.addState(new State(ts.zero()));
        efst.addArc(i + 1, new Arc(g2pmodel.getIsyms().get(se), g2pmodel.getIsyms().get(se), 0.f,
                i + 2));
        efst.setFinal(i + 2, 0.f);
        efst.setIsyms(g2pmodel.getIsyms());
        efst.setOsyms(g2pmodel.getIsyms());

        return efst;
    }

    /**
     * 
     * @param fst
     * @return
     */
    @SuppressWarnings("unchecked")
    public static ArrayList<Path> findAllPaths(Fst fst,
            HashSet<String> skipSeqs, String tie) {
        Semiring semiring = fst.getSemiring();

        ArrayList<Path> finalPaths = new ArrayList<Path>();
        HashMap<State, Path> paths = new HashMap<State, Path>();
        ArrayList<State> queue = new ArrayList<State>();
        Path p = new Path(fst.getSemiring());
        p.setCost(semiring.one());
        paths.put(fst.getStart(), p);

        queue.add(fst.getStart());

        HashMap<Integer, String> reversedOsyms = Utils.reverseHashMap(fst.getOsyms());
        while (queue.size() > 0) {
            State s = queue.get(0);
            queue.remove(0);
            if (s.getFinalWeight() != semiring.zero()) {
                finalPaths.add(paths.get(s));
            }

            for (Arc a : s.getArcs()) {
                p = new Path(fst.getSemiring());
                Path cur = paths.get(s);
                p.setCost(cur.getCost());
                p.setPath((ArrayList<String>) cur.getPath().clone());

                String sym = reversedOsyms.get(a.getOlabel());
                sym = sym.replace(tie, " ");
                if (!skipSeqs.contains(sym)) {
                    p.getPath().add(sym);
                }
                p.setCost(semiring.times(p.getCost(), a.getWeight()));
                State nextState = fst.getStateById(a.getNextStateId());
                paths.put(nextState, p);
                if (!queue.contains(nextState)) {
                    queue.add(nextState);
                }
            }
        }

        Collections.sort(finalPaths, new PathComparator());

        return finalPaths;
    }
}
