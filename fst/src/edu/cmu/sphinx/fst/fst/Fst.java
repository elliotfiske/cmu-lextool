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

package edu.cmu.sphinx.fst.fst;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;
import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <W>
 */
public class Fst<T> implements Serializable {
	
	private static final long serialVersionUID = -7821842965064006073L;
	
	// holds the fst states
	private ArrayList<State<T>> states = new ArrayList<State<T>>();
	
	// the initial state
	private Integer start = null;

	// input symbols map
	private SymbolTable isyms;
	
	// output symbols map
	private SymbolTable osyms;

	/**
	 *  Default constructor
	 */
	public Fst() {	}

	/**
	 * @return the initial state
	 */
	public Integer getStart() {
		return start;
	}

	/**
	 * @param start the initial state to set
	 */
	public void setStart(Integer start) {
		this.start = start;
	}
	
	/**
	 * Return the weight of a final state
	 * 
	 * @param sIndex the state
	 * @return the state's weight
	 */
	public Weight<T> getFinal(int sIndex) {
		return this.states.get(sIndex).getFinalWeight();
	}
	
	/**
	 * Sets the weight of a final state
	 * 
	 * @param sIndex the state
	 * @param w the state's weight
	 */
	public void setFinal(int sIndex, Weight<T> w) {
		this.states.get(sIndex).setFinalWeight(w);
	}

	/**
	 * 
	 * @return the number of states in the fst
	 */
	public int getNumStates() {
		return this.states.size();
	}

	/**
	 * Adds a state to the fst
	 * 
	 * @param s the state to be added
	 * @return the state's index 
	 */
	public int addState(State<T> s) {
		this.states.add(s);
		return states.size() - 1;
	}
	
	/**
	 * @return the states array list
	 */
	public ArrayList<State<T>> getStates() {
		return this.states; 
	}
	
	/**
	 * 
	 * @param sIndex the state's index
	 * @return the state
	 */
	public State<T> getState(int sIndex) {
		if (sIndex<this.states.size()) {
			return this.states.get(sIndex);
		}
		
		return null;
	}

	/**
	 * 
	 * @param sIndex
	 * @param s
	 */
	public void setState(int sIndex, State<T> s) {
		if (sIndex < this.states.size()) {
			this.states.set(sIndex, s);
		}
	}

	/**
	 * @return the isyms
	 */
	public SymbolTable getIsyms() {
		return isyms;
	}

	/**
	 * @param isyms the isyms to set
	 */
	public void setIsyms(SymbolTable isyms) {
		this.isyms = isyms;
	}

	/**
	 * @return the osyms
	 */
	public SymbolTable getOsyms() {
		return osyms;
	}

	/**
	 * @param osyms the osyms to set
	 */
	public void setOsyms(SymbolTable osyms) {
		this.osyms = osyms;
	}
	
	/**
	 * adds a new outgoing arc from a state 
	 * 
	 * @param stateId the state's id
	 * @param arc the arc
	 */
	public void addArc(int stateId, Arc<T> arc) {
		State<T> s = states.get(stateId);
		s.getArcs().add(arc);
	}

	/**
	 *  saves compressed binary model to disk
	 * @param filename the binary model filename
	 * @throws IOException
	 */
	public void saveModel(String filename) throws IOException {
		FileOutputStream fos = new FileOutputStream(filename);
		GZIPOutputStream gos = new GZIPOutputStream(fos);
		ObjectOutputStream oos = new ObjectOutputStream(gos);
		oos.writeObject(this);
		oos.flush();
        oos.close();
        gos.close();
        fos.close();
	}
	
	/**
	 * Loads a compressed binary model from disk
	 * @param filename the binary model filename
	 * @throws IOException
	 * @throws ClassNotFoundException 
	 */
	public static Object loadModel(String filename) {
		Object obj = null;
	    FileInputStream fis = null;
	    GZIPInputStream gis = null;
	    ObjectInputStream ois = null;
	    
	    try {
			fis = new FileInputStream(filename);
			gis = new GZIPInputStream(fis);
	        ois = new ObjectInputStream(gis);
	        obj = ois.readObject();
	        ois.close();
	        gis.close();
	        fis.close();
		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		} catch (ClassNotFoundException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
			System.exit(1);
		}

        return obj;		
	}

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
		@SuppressWarnings("unchecked")
		Fst<T> other = (Fst<T>) obj;
		if (isyms == null) {
			if (other.isyms != null)
				return false;
		} else if (!isyms.equals(other.isyms))
			return false;
		if (osyms == null) {
			if (other.osyms != null)
				return false;
		} else if (!osyms.equals(other.osyms))
			return false;
		if (start == null) {
			if (other.start != null)
				return false;
		} else if (!start.equals(other.start))
			return false;
		if (states == null) {
			if (other.states != null)
				return false;
		} else if (!states.equals(other.states))
			return false;
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "Fst [states=" + states + ", start=" + start + ", isyms="
				+ isyms + ", osyms=" + osyms + "]";
	}
		
	 
	/**
	 * A quick way to perform a deep copy of an existing FST:
	 * just serializing it and deserialize again on a new FST.
	 * @return
	 */
	@SuppressWarnings("unchecked")
	public Fst<T> copy() {
		Fst<T> fst = null;
        try {
            // Write the object out to a byte array
            ByteArrayOutputStream bos = new ByteArrayOutputStream();
            ObjectOutputStream out = new ObjectOutputStream(bos);
            out.writeObject(this);
            out.flush();
            out.close();

            // Make an input stream from the byte array and read
            // a copy of the object back in.
            ObjectInputStream in = new ObjectInputStream(
                new ByteArrayInputStream(bos.toByteArray()));
            fst = (Fst<T>) in.readObject();
        }
        catch(IOException e) {
            e.printStackTrace();
        }
        catch(ClassNotFoundException cnfe) {
            cnfe.printStackTrace();
        }
        return fst;
    }
}

