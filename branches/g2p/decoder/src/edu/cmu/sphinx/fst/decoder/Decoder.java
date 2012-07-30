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
import edu.cmu.sphinx.fst.operations.Compose;
import edu.cmu.sphinx.fst.operations.NShortestPaths;
import edu.cmu.sphinx.fst.operations.Project;
import edu.cmu.sphinx.fst.operations.ProjectType;
import edu.cmu.sphinx.fst.operations.RmEpsilon;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.semiring.TropicalSemiring;
import edu.cmu.sphinx.fst.utils.Mapper;
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
    Mapper<Vector<String>, Integer> clusters = new Mapper<Vector<String>, Integer>();

    Fst g2pmodel;
    Fst epsilonFilter;
    Mapper<Integer, String> isyms;

    public Decoder(String g2pmodel_file) {
        skipSeqs.add(eps);
        skipSeqs.add(sb);
        skipSeqs.add(se);
        skipSeqs.add(skip);
        skipSeqs.add("-");

        g2pmodel = Fst.loadModel(g2pmodel_file);

        // keep an augmented copy (for compose)
        Compose.augment(0, g2pmodel, g2pmodel.getSemiring());

        isyms = g2pmodel.getIsyms();
        tie = isyms.getValue(1); // The separator symbol is reserved for index 1

        loadClusters();

        // get epsilon filter for composition
        epsilonFilter = Compose.getFilter(isyms, g2pmodel.getSemiring());
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
        for (int i = 2; i < isyms.size(); i++) {
            String sym = isyms.getValue(i);
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
        Fst result = Compose.compose(efst, epsilonFilter, s);
        result = Compose.compose(result, g2pmodel, s);
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
        efst.addArc(s.getId(), new Arc(isyms.getKey(sb), isyms.getKey(sb), 0.f,
                "1"));
        efst.setStart(s.getId());

        // Build the basic FSA
        int i;
        for (i = 0; i < entry.size(); i++) {
            String str = entry.get(i);
            s = new State(ts.zero());
            efst.addState(s);
            efst.addArc(s.getId(), new Arc(isyms.getKey(str),
                    isyms.getKey(str), 0.f, Integer.toString(i + 2)));
        }

        // Add any cluster arcs
        for (Integer value : clusters.valueSet()) {
            Vector<String> cluster = clusters.getKey(value);
            int start = 0;
            int k = 0;
            while (k != -1) {
                k = Utils.search(entry, cluster, start);
                if (k != -1) {
                    efst.addArc(
                            Integer.toString(start + k + 1),
                            new Arc(value, value, 0.f, Integer.toString(start
                                    + k + cluster.size() + 1)));
                    start = start + k + cluster.size();
                }
            }
        }

        efst.addState(new State(ts.zero()));
        efst.addState(new State(ts.zero()));
        efst.addArc(
                Integer.toString(i + 1),
                new Arc(isyms.getKey(se), isyms.getKey(se), 0.f, Integer
                        .toString(i + 2)));
        efst.setFinal(Integer.toString(i + 2), 0.f);
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

                String sym = fst.getOsyms().getValue(a.getOlabel());
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
