/*
 * Copyright 1999-2013 Carnegie Mellon University.
 * Portions Copyright 2002 Sun Microsystems, Inc.
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.result;

import java.util.ArrayList;
import java.util.HashSet;

/**
 * NBest list with A*
 */

public class Nbest {

    protected Lattice lattice;

    public Nbest(Lattice lattice) {
        this.lattice = lattice;
    }

    class NBestPath implements Comparable<NBestPath> {
        String path;
        Node node;
        double score;
        double forwardScore;

        public NBestPath(String path, Node node, double score,
                double forwardScore) {
            super();
            this.path = path;
            this.node = node;
            this.score = score;
            this.forwardScore = forwardScore;
        }

        @Override
        public int compareTo(NBestPath o) {
            return Double.compare(score, o.score);
        }

        public String toString() {
            return path + " [" + score + ',' + forwardScore + ']';
        }
    }

    public ArrayList<String> getNbest(int n) {
        lattice.computeNodePosteriors(1.0f);
        HashSet<String> result = new HashSet<String>();

        BoundedPriorityQueue<NBestPath> queue = new BoundedPriorityQueue<Nbest.NBestPath>(
                n);

        queue.add(new NBestPath("<s>", lattice.getInitialNode(), 0, 0));

        while (result.size() < n && queue.size() > 0) {
            NBestPath path = queue.poll();
            if (path.node.equals(lattice.terminalNode)) {
                result.add(path.path);
                continue;
            }

            for (Edge e : path.node.getLeavingEdges()) {
                Node newNode = e.getToNode();
                double newScore = path.forwardScore
                        + newNode.getBackwardScore();
                double newForwardScore = path.forwardScore
                        + e.getAcousticScore() + e.getLMScore();
                NBestPath newPath = new NBestPath(path.path + " "
                        + newNode.getWord(), newNode, newScore, newForwardScore);
                queue.add(newPath);
            }
            // printQueue(queue);
        }
        return new ArrayList<String>(result);
    }

    @SuppressWarnings("unused")
    private void printQueue(BoundedPriorityQueue<NBestPath> queue) {
        System.out.println();
        for (NBestPath p : queue) {
            System.out.println(p);
        }
    }
}
