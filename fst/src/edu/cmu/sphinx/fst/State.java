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

package edu.cmu.sphinx.fst;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class State {

    // Id
    protected int id = -1;

    // Final weight
    private float fnlWeight;

    // Outgoing arcs collection
    private ArrayList<Arc> arcs = null;
    
    protected int initialNumArcs = -1;
    
    public State() {
      arcs = new ArrayList<Arc>();
    }
    
    /**
     * 
     * @param fnlWeight
     */
    public State(float fnlWeight) {
        this();
        this.fnlWeight = fnlWeight;
    }



    public State(int initialNumArcs) {
        this.initialNumArcs = initialNumArcs;
        if(initialNumArcs > 0) {
            arcs = new ArrayList<Arc>(initialNumArcs);
        }
    }
    
    
    /**
     * 
     * @param cmp
     */
    public void arcSort(Comparator<Arc> cmp) {
        Collections.sort(arcs, cmp);
    }

    /**
     * @return the Final weight
     */
    public float getFinalWeight() {
        return fnlWeight;
    }

    /**
     * @param arcs the arcs to set
     */
    public void setArcs(ArrayList<Arc> arcs) {
        this.arcs = arcs;
    }

    /**
     * @param fnlfloat the Final weight to set
     */
    public void setFinalWeight(float fnlfloat) {
        this.fnlWeight = fnlfloat;
    }

    /**
     * @return the id
     */
    public int getId() {
        return id;
    }

    /**
     * @param id the id to set
     */
    /**
     * 
     * @return the number of outgoing arcs
     */
    public int getNumArcs() {
        return this.arcs.size();
    }

    /**
     * Adds an arc
     * 
     * @param arc the arc to add
     * @return the arc's index
     */
    public void addArc(Arc arc) {
        this.arcs.add(arc);
    }

    /**
     * 
     * @param aIndex the arc's index
     * @return the arc
     */
    public Arc getArc(int index) {
        return this.arcs.get(index);
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (obj == null)
            return false;
        if (getClass() != obj.getClass())
            return false;
        State other = (State) obj;
        if (id != other.id)
            return false;
        if (!(fnlWeight == other.fnlWeight)) {
            if (Float.floatToIntBits(fnlWeight) != Float
                    .floatToIntBits(other.fnlWeight))
                return false;
        }
        if (arcs == null) {
            if (other.arcs != null)
                return false;
        } else if (!arcs.equals(other.arcs))
            return false;
        return true;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("(" + id + ", " + fnlWeight + ")");
        return sb.toString();
    }

    // delete an arc
    public Arc deleteArc(int index) {
        return this.arcs.remove(index);
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + id;
        // result = prime * result + Float.floatToIntBits(fnlWeight);
        // result = prime * result + ((arcs == null) ? 0 : arcs.hashCode());
        return result;
    }

}
