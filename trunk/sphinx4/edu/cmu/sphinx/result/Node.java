/*
 * Copyright 1999-2002 Carnegie Mellon University.
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

import edu.cmu.sphinx.result.Lattice;
import edu.cmu.sphinx.result.Edge;
import edu.cmu.sphinx.util.LogMath;

import java.util.LinkedList;
import java.util.List;
import java.util.StringTokenizer;
import java.util.Iterator;
import java.util.Vector;
import java.util.Collection;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;

/**
 * Nodes are part of Lattices.  The represent theories that words were spoken over a given time.
 */
public class Node {
    protected static int nodeCount = 0; // used to generate unique IDs for new Nodes.

    protected String id;
    protected String word;
    protected int beginTime = -1;
    protected int endTime = -1;
    protected Vector fromEdges;
    protected Vector toEdges;
    protected double forwardScore;
    protected double backwardScore;
    protected double posterior;
    
    {
        fromEdges = new Vector();
        toEdges = new Vector();
        nodeCount++;
    }

    /**
     * Create a new Node
     *
     * @param word
     * @param beginTime
     * @param endTime
     */
    protected Node(String word, int beginTime, int endTime) {
        this(getNextNodeId(), word, beginTime, endTime);
    }

    /**
     * Create a new Node with given ID.  Used when creating a Lattice from a .LAT file
     *
     * @param id
     * @param word
     * @param beginTime
     * @param endTime
     */
    protected Node(String id, String word, int beginTime, int endTime) {
        this.id = id;
        this.word = word;
        this.beginTime = beginTime;
        this.endTime = endTime;
        this.forwardScore = LogMath.getLogZero();
        this.backwardScore = LogMath.getLogZero();
        this.posterior = LogMath.getLogZero();
    }

    /**
     * Get a unique ID for a new Node.
     * Used when creating a Lattice from a .LAT file
     *
     * @return the unique ID for a new node
     */
    protected static String getNextNodeId() {
        return Integer.toString(nodeCount);
    }

    /**
     * Test if a node has an Edge to a Node
     * @param n
     * @return unique Node ID
     */
    protected boolean hasEdgeToNode(Node n) {
        return getEdgeToNode(n) != null;
    }

    /**
     * given a node find the edge to that node
     *
     * @param n the node of interest
     *
     * @return the edge to that node or <code> null</code>  if no edge
     * could be found.
     */
    public Edge getEdgeToNode(Node n) {
        for (Iterator j = toEdges.iterator(); j.hasNext();) {
            Edge e = (Edge) j.next();
            if (e.getToNode() == n) {
                return e;
            }
        }
        return null;
    }

    /**
     * Test is a Node has an Edge from a Node
     *
     * @param n
     * @return true if this node has an Edge from n
     */
    protected boolean hasEdgeFromNode(Node n) {
        return getEdgeFromNode(n) != null;
    }

    /**
     * given a node find the edge from that node
     *
     * @param n the node of interest
     *
     * @return the edge from that node or <code> null</code>  if no edge
     * could be found.
     */
    public Edge getEdgeFromNode(Node n) {
        for (Iterator j = fromEdges.iterator(); j.hasNext();) {
            Edge e = (Edge) j.next();
            if (e.getFromNode() == n) {
                return e;
            }
        }
        return null;
    }

    /**
     * Test if a Node has all Edges from the same Nodes and another Node.
     *
     * @param n
     * @return true if this Node has Edges from the same Nodes as n
     */
    protected boolean hasEquivalentFromEdges(Node n) {
        if (fromEdges.size() != n.getFromEdges().size()) {
            return false;
        }
        for (Iterator i = fromEdges.iterator(); i.hasNext();) {
            Edge e = (Edge) i.next();
            Node fromNode = e.getFromNode();
            if (!n.hasEdgeFromNode(fromNode)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Test if a Node has all Edges to the same Nodes and another Node.
     *
     * @param n the node of interest
     * @return true if this Node has all Edges to the sames Nodes as n
     */
    public boolean hasEquivalentToEdges(Node n) {
        if (toEdges.size() != n.getToEdges().size()) {
            return false;
        }
        for (Iterator i = toEdges.iterator(); i.hasNext();) {
            Edge e = (Edge) i.next();
            Node toNode = e.getToNode();
            if (!n.hasEdgeToNode(toNode)) {
                return false;
            }
        }
        return true;
    }

    /**
     * Get the Edges from this Node
     *
     * @return Edges from this Node
     */
    public Collection getFromEdges() {
        return fromEdges;
    }

    /**
     * Get the Edges to this Node
     *
     * @return Edges to this Node
     */
    public Collection getToEdges() {
        return toEdges;
    }

    /**
     * Add an Edge from this Node
     *
     * @param e
     */
    protected void addFromEdge(Edge e) {
        fromEdges.add(e);
    }

    /**
     * Add an Edge to this Node
     *
     * @param e
     */
    protected void addToEdge(Edge e) {
        toEdges.add(e);
    }

    /**
     * Remove an Edge from this Node
     *
     * @param e
     */
    protected void removeFromEdge(Edge e) {
        fromEdges.remove(e);
    }

    /**
     * Remove an Edge to this Node
     *
     * @param e the edge to remove
     */
    public void removeToEdge(Edge e) {
        toEdges.remove(e);
    }

    /**
     * Get the ID associated with this Node
     *
     * @return the ID
     */
    public String getId() {
        return id;
    }

    /**
     * Get the word associated with this Node
     *
     * @return the word
     */
    public String getWord() {
        return word;
    }

    /**
     * Get the frame number when the word began
     *
     * @return the begin frame number, or -1 if the frame number is unknown
     */
    public int getBeginTime() {
        return beginTime;
    }

    /**
     * Get the frame number when the word ends
     *
     * @return the end time, or -1 if the frame number if is unknown
     */
    public int getEndTime() {
        return endTime;
    }

    public String toString() {
        return "Node(" + word + "," + getBeginTime() + "|" + getEndTime() + ")";
    }

    /**
     * Internal routine when dumping Lattices as AiSee files
     *
     * @param f
     * @throws IOException
     */
    void dumpAISee(FileWriter f) throws IOException {
        f.write("node: { title: \"" + id + "\" label: \""
                + getWord() + "[" + getBeginTime() + "," + getEndTime() + 
                " p:" + getPosterior() + "]\" }\n");
    }

    /**
     * Internal routine used when dumping Lattices as .LAT files
     * @param f
     * @throws IOException
     */
    void dump(PrintWriter f) throws IOException {
        f.println("node: " + id + " " + word + 
                //" a:" + getForwardProb() + " b:" + getBackwardProb()
                " p:" + getPosterior());
    }

    /**
     * Internal routine used when loading Lattices from .LAT files
     * @param lattice
     * @param tokens
     */
    static void load(Lattice lattice, StringTokenizer tokens) {

        String id = tokens.nextToken();
        String label = tokens.nextToken();

        lattice.addNode(id, label, 0, 0);
    }
    /**
     * @return Returns the backwardScore.
     */
    public double getBackwardScore() {
        return backwardScore;
    }
    /**
     * @param backwardScore The backwardScore to set.
     */
    public void setBackwardScore(double backwardScore) {
        this.backwardScore = backwardScore;
    }
    /**
     * @return Returns the forwardScore.
     */
    public double getForwardScore() {
        return forwardScore;
    }
    /**
     * @param forwardScore The forwardScore to set.
     */
    public void setForwardScore(double forwardScore) {
        this.forwardScore = forwardScore;
    }
    /**
     * @return Returns the posterior probability of this node.
     */
    public double getPosterior() {
        return posterior;
    }
    /**
     * @param posterior The node posterior probability to set.
     */
    public void setPosterior(double posterior) {
        this.posterior = posterior;
    }
    
    /**
     * @see java.lang.Object#hashCode()
     */
    public int hashCode() {
        return id.hashCode();
    }
    
    
    /**
     * Assumes ids are unique node identifiers
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    public boolean equals(Object obj) {
        return id.equals(((Node)obj).getId());
    }
    
    public void calculateBeginTime() {
        beginTime = 0;
        Iterator e = fromEdges.iterator();
        while (e.hasNext()) {
            Edge edge = (Edge)e.next();
            if (edge.getFromNode().getEndTime() > beginTime) {
                beginTime = edge.getFromNode().getEndTime();
            }
        }
    }
        
    /**
     * Get the nodes at the other ends of outgoing edges of this node.
     * 
     * @return a list of child nodes
     */
    public List getChildNodes() {
        LinkedList childNodes = new LinkedList();
        Iterator e = toEdges.iterator();
        while (e.hasNext()) {
            Edge edge = (Edge)e.next();
            childNodes.add(edge.getToNode());
        }
        return childNodes;
    }
    
    protected boolean isAncestorHelper(List children, Node node) {
        Iterator i = children.iterator();
        while(i.hasNext()) {
            Node n = (Node)i.next();
            if (n.equals(node)) {
                return true;
            }
            if (isAncestorHelper(n.getChildNodes(),node)) {
                return true;
            }
        }
        return false;
    }
    
    /**
     * Check whether this node is an ancestor of another node.
     * 
     * @param node the Node to check
     * @return whether this node is an ancestor of the passed in node.
     */
    public boolean isAncestorOf(Node node) {
        if (this.equals(node)) {
            return true; // node is its own ancestor
        }
        return isAncestorHelper(this.getChildNodes(),node);
    }
    
    /**
     * Check whether this node has an ancestral relationship with another node
     * (i.e. either this node is an ancestor of the other node, or vice versa)
     * 
     * @param node the Node to check for a relationship
     * @return whether a relationship exists
     */
    public boolean hasAncestralRelationship(Node node) {
        return this.isAncestorOf(node) || node.isAncestorOf(this);
    }
}
