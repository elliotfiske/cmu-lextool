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
import edu.cmu.sphinx.fst.ImmutableFst;
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
    Vector<String>[] clusters = null;

    ImmutableFst g2pmodel;
    Fst epsilonFilter;

    public Decoder(String g2pmodel_file) {
        skipSeqs.add(eps);
        skipSeqs.add(sb);
        skipSeqs.add(se);
        skipSeqs.add(skip);
        skipSeqs.add("-");

        g2pmodel = ImmutableFst.loadModel(g2pmodel_file);
        // keep an augmented copy (for compose)
        Compose.augment(0, g2pmodel, g2pmodel.getSemiring());
        ArcSort.apply(g2pmodel, new ILabelCompare());

        String[] isyms = g2pmodel.getIsyms();
        tie = isyms[1]; // The separator symbol is reserved for
                        // index 1

        loadClusters(isyms);

        // get epsilon filter for composition
        epsilonFilter = Compose.getFilter(g2pmodel.getIsyms(),
                g2pmodel.getSemiring());
        ArcSort.apply(epsilonFilter, new ILabelCompare());
    }

    /**
     * 
     */
    @SuppressWarnings("unchecked")
    private void loadClusters(String[] syms) {
        clusters = new Vector[syms.length];
        for (int i = 0; i < syms.length; i++) {
            clusters[i] = null;
        }
        for (int i = 2; i < syms.length; i++) {
            String sym = syms[i];
            if (sym.contains(tie)) {
                Vector<String> tmp = Utils.split_string(sym, tie);
                Vector<String> cluster = new Vector<String>();
                for (int j = 0; j < tmp.size(); j++) {
                    if (!tmp.get(j).equals(tie)) {
                        cluster.add(tmp.get(j));
                    }
                }
                clusters[i] = cluster;
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
        efst.setStart(s);

        // Build the basic FSA
        for (int i = 0; i < entry.size() + 1; i++) {
            s = new State(ts.zero());
            efst.addState(s);
            if (i >= 1) {
                int symIndex = Utils.getIndex(g2pmodel.getIsyms(),
                        entry.get(i - 1));
                efst.getState(i).addArc(new Arc(symIndex, symIndex, 0.f, s));
            } else if (i == 0) {
                int symIndex = Utils.getIndex(g2pmodel.getIsyms(), sb);
                efst.getStart().addArc(new Arc(symIndex, symIndex, 0.f, s));
            }

            if (i == entry.size()) {
                State s1 = new State(ts.zero());
                efst.addState(s1);
                int symIndex = Utils.getIndex(g2pmodel.getIsyms(), se);
                s.addArc(new Arc(symIndex, symIndex, 0.f, s1));
                s1.setFinalWeight(0.f);
            }
        }

        // Add any cluster arcs
        for (int value = 0; value < clusters.length; value++) {
            Vector<String> cluster = clusters[value];
            if (cluster != null) {
                int start = 0;
                int k = 0;
                while (k != -1) {
                    k = Utils.search(entry, cluster, start);
                    if (k != -1) {
                        State from = efst.getState(start + k + 1);
                        from.addArc(new Arc(value, value, 0.f, efst
                                .getState(start + k + cluster.size() + 1)));
                        start = start + k + cluster.size();
                    }
                }
            }
        }

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

        String[] osyms = fst.getOsyms();
        while (queue.size() > 0) {
            State s = queue.get(0);
            queue.remove(0);
            if (s.getFinalWeight() != semiring.zero()) {
                finalPaths.add(paths.get(s));
            }

            int numArcs = s.getNumArcs();
            for (int j = 0; j < numArcs; j++) {
                Arc a = s.getArc(j);
                p = new Path(fst.getSemiring());
                Path cur = paths.get(s);
                p.setCost(cur.getCost());
                p.setPath((ArrayList<String>) cur.getPath().clone());

                String sym = osyms[a.getOlabel()];
                sym = sym.replace(tie, " ");
                if (!skipSeqs.contains(sym)) {
                    p.getPath().add(sym);
                }
                p.setCost(semiring.times(p.getCost(), a.getWeight()));
                State nextState = a.getNextState();
                paths.put(nextState, p);
                if (!queue.contains(nextState)) {
                    queue.add(nextState);
                }
            }
        }

        Collections.sort(finalPaths, new PathComparator());

        return finalPaths;
    }

    public String[] getModelIsyms() {
        return g2pmodel.getIsyms();
    }
}
