/**
 * 
 */
package edu.cmu.sphinx.fst.weight;

import java.io.Serializable;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public abstract class AbstactSemiring<T extends Comparable<T>> implements Semiring<T>, Serializable  {
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;

	/* (non-Javadoc)
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
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return this.getClass().toString();
	}
	
	/**
	 * NATURAL ORDER
	 * 
	 * By definition:
	 *           a <= b iff a + b = a
	 * 
	 * The natural order is a negative partial order iff the semiring is idempotent. 
	 * It is trivially monotonic for plus. It is left (resp. right) monotonic for 
	 * times iff the semiring is left (resp. right) distributive. It is a total order 
	 * iff the semiring has the path property.
	 * 
	 * See Mohri, "Semiring Framework and Algorithms for Shortest-Distance Problems", 
	 * Journal of Automata, Languages and Combinatorics 7(3):321-350, 2002. 
	 * 
	 * We define the strict version of this order below. 
	 * 
	 */
	public boolean naturalLess(Weight<T> w1, Weight<T> w2) {
		return (this.plus(w1, w2).equals(w1)) && (!w1.equals(w2));
	}
	
	
}
