/**
 * 
 */
package edu.cmu.sphinx.fst;

import java.util.Arrays;
import java.util.Comparator;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class ImmutableState extends State {
    private Arc[] arcs = null;
    
    protected ImmutableState(int numArcs) {
        super(0);
        this.initialNumArcs = numArcs;
        arcs = new Arc[numArcs];
    }

    public void arcSort(Comparator<Arc> cmp) {
        Arrays.sort(arcs, cmp);
    }

    /**
     * @return the arcs
     */

    @Override
    public void addArc(Arc arc) {
        throw new IllegalArgumentException("You cannot modify an ImmutableState.");
    }

    public void setArc(int index, Arc arc) {
        arcs[index] = arc; 
    }


    @Override
    public Arc deleteArc(int index) {
        throw new IllegalArgumentException("You cannot modify an ImmutableState.");
    }

    /**
     * @param arcs the arcs to set
     */
    public void setArcs(Arc[] arcs) {
        this.arcs = arcs;
    }

    @Override
    public int getNumArcs() {
        return initialNumArcs;
    }

    @Override
    public Arc getArc(int index) {
        return this.arcs[index];
    }

    /* (non-Javadoc)
     * @see java.lang.Object#hashCode()
     */
    @Override
    public int hashCode() {
        final int prime = 31;
        int result = 1;
        result = prime * result + id;
        return result;
    }

    /* (non-Javadoc)
     * @see java.lang.Object#equals(java.lang.Object)
     */
    @Override
    public boolean equals(Object obj) {
        if (this == obj)
            return true;
        if (getClass() != obj.getClass())
            return false;
        ImmutableState other = (ImmutableState) obj;
        if (!Arrays.equals(arcs, other.arcs))
            return false;
        if (!super.equals(obj))
            return false;
        return true;
    }
    
    
}
