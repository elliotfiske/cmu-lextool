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

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 * @param <W>
 */
public class Arc {

    // Arc's weight
    private float weight;

    // input label
    private int iLabel;

    // output label
    private int oLabel;

    // next state's id
    private State nextState;

    public Arc() {
    }

    /**
     * Creates a new arc
     * 
     * @param iLabel the input label's id
     * @param oLabel the output label's id
     * @param weight the arc's weight
     * @param nextStateId the next state's id
     */
    public Arc(int iLabel, int oLabel, float weight, State nextState) {
        this.weight = weight;
        this.iLabel = iLabel;
        this.oLabel = oLabel;
        this.nextState = nextState;
    }

    /**
     * Get the arc's weight
     * 
     * @return the weight
     */
    public float getWeight() {
        return weight;
    }

    /**
     * Set the arc's weight
     * 
     * @param weight the weight to set
     */
    public void setWeight(float weight) {
        this.weight = weight;
    }

    /**
     * Get the input label's id
     * 
     * @return the input label's id
     */
    public int getIlabel() {
        return iLabel;
    }

    /**
     * Set the input label's id
     * 
     * @param iLabel the input label's to set
     */
    public void setIlabel(int iLabel) {
        this.iLabel = iLabel;
    }

    /**
     * Get the output label's id
     * 
     * @return the output label's id
     */
    public int getOlabel() {
        return oLabel;
    }

    /**
     * Set the output label's id
     * 
     * @param oLabel the output label's to set
     */
    public void setOlabel(int oLabel) {
        this.oLabel = oLabel;
    }

    /**
     * Get the next state's id
     * 
     * @return the next state's id
     */
    public State getNextState() {
        return nextState;
    }

    /**
     * Set the next state's id
     * 
     * @param nextStateId the next state's id to set
     */
    public void setNextState(State nextState) {
        this.nextState = nextState;
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
        Arc other = (Arc) obj;
        if (iLabel != other.iLabel)
            return false;
        if (nextState == null) {
            if (other.nextState != null)
                return false;
        } else if (nextState.getId() != other.nextState.getId())
            return false;
        if (oLabel != other.oLabel)
            return false;
        if (!(weight == other.weight)) {
            if (Float.floatToIntBits(weight) != Float
                    .floatToIntBits(other.weight))
                return false;
        }
        return true;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "(" + iLabel + ", " + oLabel + ", " + weight + ", " + nextState
                + ")";
    }
}
