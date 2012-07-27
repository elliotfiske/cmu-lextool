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

import java.io.Externalizable;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInput;
import java.io.ObjectInputStream;
import java.io.ObjectOutput;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <W>
 */
public class Fst<T extends Comparable<T>> implements Externalizable {
	
	private static final long serialVersionUID = -7821842965064006073L;
	
	// holds the fst states
	private ArrayList<State<T>> states = new ArrayList<State<T>>();
	
	// the initial state
	private String start = null;

	// input symbols map
	private Mapper<Integer, String> isyms;
	
	// output symbols map
	private Mapper<Integer, String> osyms;
	
	// state symbols map (maps the states' ids to the index in the array)
	private Mapper<Integer, String> ssyms = new Mapper<Integer, String>();
	
	// holds the semiring
	private Semiring<T> semiring;

	/**
	 *  Default constructor
	 */
	public Fst() {}

	public Fst(Semiring<T> s) {
		this.semiring = s;
	}

	/**
	 * @return the initial state
	 */
	public String getStartId() {
		return start;
	}

	/**
	 * @return the initial state
	 */
	public State<T> getStart() {
		return states.get(ssyms.getKey(start));
	}
	
	
	/**
	 * @return the semiring
	 */
	public Semiring<T> getSemiring() {
		return semiring;
	}

	/**
	 * @return the ssyms
	 */
	public Mapper<Integer, String> getSsyms() {
		return ssyms;
	}

	/**
	 * @param semiring the semiring to set
	 */
	public void setSemiring(Semiring<T> semiring) {
		this.semiring = semiring;
	}

	/**
	 * @param stateId the initial state to set
	 */
	public void setStart(String stateId) {
		if(ssyms.getKey(stateId) == null) {
			System.out.println("Cannot find state.");
			return;
		}
		this.start = stateId;
	}
	
	/**
	 * Sets the weight of a final state
	 * 
	 * @param sIndex the state
	 * @param w the state's weight
	 */
	public void setFinal(String stateId, Weight<T> w) {
		if(ssyms.getKey(stateId) == null) {
			System.out.println("Cannot find state.");
			return;
		}

		this.states.get(ssyms.getKey(stateId)).setFinalWeight(w);
	}

	/**
	 * 
	 * @return the number of states in the fst
	 */
	public int getNumStates() {
		return this.states.size();
	}

	/**
	 * @return the states
	 */
	public Iterator<State<T>> stateIterator() {
		return states.iterator();
	}

	
	/**
	 * Adds a state to the fst
	 * 
	 * @param s the state to be added
	 * @return the state's index 
	 */
	public String addState(State<T> state) {
		state.setSemiring(this.semiring);
		if(ssyms.getKey(state.getId()) != null) {
			System.out.println("State is already in the fst;");
			return state.getId();
		}
		if (state.getId() == null) {
			state.setId(Integer.toString(states.size()));
		}
		
		ssyms.put(states.size(), state.getId());
		this.states.add(state);
		
		return state.getId(); 
	}
	
	
	/**
	 * 
	 * @param sIndex the state's index
	 * @return the state
	 */
	public State<T> getStateById(String stateId) {
		if(ssyms.getKey(stateId) == null) {
			return null;
		}

		return this.states.get(ssyms.getKey(stateId));
	}

	public State<T> getStateByIndex(int index) {
		return this.states.get(index);
	}

	/**
	 * @return the isyms
	 */
	public Mapper<Integer, String> getIsyms() {
		return isyms;
	}

	/**
	 * @param isyms the isyms to set
	 */
	public void setIsyms(Mapper<Integer, String> isyms) {
		this.isyms = isyms;
	}

	/**
	 * @return the osyms
	 */
	public Mapper<Integer, String>  getOsyms() {
		return osyms;
	}

	/**
	 * @param osyms the osyms to set
	 */
	public void setOsyms(Mapper<Integer, String>  osyms) {
		this.osyms = osyms;
	}
	
	/**
	 * adds a new outgoing arc from a state 
	 * 
	 * @param stateId the state's id
	 * @param arc the arc
	 */
	public void addArc(String stateId, Arc<T> arc) {
		if(ssyms.getKey(stateId) == null) {
			System.out.println("Cannot find state.");
			return;
		}

		State<T> s = states.get(ssyms.getKey(stateId));
		s.addArc(arc);
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
	@SuppressWarnings("unchecked")
	public static<T extends Comparable<T>> Fst<T> loadModel(String filename) {
		Fst<T> obj = null;
        
		FileInputStream fis = null;
	    GZIPInputStream gis = null;
	    ObjectInputStream ois = null;
	    
	    try {
	    	fis = new FileInputStream(filename);
			gis = new GZIPInputStream(fis);
	        ois = new ObjectInputStream(gis);
	        obj = (Fst<T>) ois.readObject();
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
		} else {
			// check ssyms
			if (ssyms == null) {
				if (other.ssyms != null)
					return false;
			} else if (ssyms.equals(other.ssyms)) {
				// equal ssyms means equally indexing on array
				if (!states.equals(other.states))
					return false;
			} else {
				if(ssyms.size() != other.ssyms.size())
					return false;
				// ssyms have same size. check states by looking up index on ssyms
				Integer key;
				String id;
				State<T> s;
				State<T> others;
				for (Iterator<Integer> it = ssyms.keySet().iterator(); it.hasNext();) {
					key = it.next();
					id = ssyms.getValue(key);
					s = getStateByIndex(key);
					others = other.getStateById(id);
					if(!s.equals(others))
						return false;
				}
			}
		}
		if (semiring == null) {
			if (other.semiring != null)
				return false;
		} else if (!semiring.equals(other.semiring))
			return false;
		return true;
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		StringBuilder sb = new StringBuilder();
		sb.append("Fst(start=" + start + ", isyms=" + isyms + ", osyms=" + osyms + ", ssyms=" + ssyms + ", semiring=" + semiring + ")\n");
		State<T> s;
		Arc<T> a;
		for(Iterator<State<T>> itS = states.iterator(); itS.hasNext();) {
			s=itS.next();
					
			sb.append("  " +s+"\n");
			for(Iterator<Arc<T>> itA = s.arcIterator(); itA.hasNext();) {
				a = itA.next();
				sb.append("    " +a+"\n");
			}

		}

		return sb.toString();
	}

	private static Mapper<Integer, String> copySyms(Mapper<Integer, String> syms) {
		Mapper<Integer, String> newsyms = new Mapper<Integer, String>();
		
		for(Iterator<Integer> it = syms.keySet().iterator(); it.hasNext();) {
			Integer key = it.next(); 
			newsyms.put(key, syms.getValue(key));
		}
		return newsyms; 
	}
	 
	/**
	 * @return
	 */
	public Fst<T> copy() {
		Fst<T> fst = new Fst<T>(this.getSemiring());
		fst.setIsyms(copySyms(this.isyms));
		fst.setOsyms(copySyms(this.osyms));
		
		State<T> oldState;
		State<T> newState;
		Arc<T> oldArc;
		Arc<T> newArc;
		for(Iterator<State<T>> itS = states.iterator(); itS.hasNext();) {
			oldState = itS.next();
			newState = new State<T>(oldState.getFinalWeight());
			newState.setId(oldState.getId());
			fst.addState(newState);
			for(Iterator<Arc<T>> itA = oldState.arcIterator(); itA.hasNext();) {
				oldArc = itA.next();
				newArc = new Arc<T>(oldArc.getIlabel(), oldArc.getOlabel(), oldArc.getWeight(), oldArc.getNextStateId());
				newState.addArc(newArc);
			}
		}
		fst.setStart(this.start);
		
        return fst;
    }
	
	/**
	 * Remaps the states
	 */
	private void remapStates() {
		// reset symbols table
		ssyms = new Mapper<Integer, String>();
		
		//remap states
		for(int i=0; i<states.size(); i++) {
			State<T> s = states.get(i);
			ssyms.put(i, s.getId());
		}
	}
	/**
	 * Delete a state
	 * @param i the index of the state
	 */
	public void deleteState(String stateId) {
		if(stateId == this.start) {
			System.out.println("Cannot delete start state.");
			return;
		}

		if(ssyms.getKey(stateId) == null) {
			System.out.println("Cannot find state.");
			return;
		}
		int index2 = ssyms.getKey(stateId);
		this.states.remove(index2);
		remapStates();
		
		// delete arc's with nextstate equal to stateid
		State<T> s;
		Arc<T> arc;
		for(Iterator<State<T>> itState = this.states.iterator(); itState.hasNext();) {
			s = itState.next();
			ArrayList<Integer> toDelete = new ArrayList<Integer>();
//			for(int i=0; i< s.getNumArcs(); i++) {
			for(int i=0; i< s.getNumArcs(); i++) {
				arc = s.getArc(i);
				if(arc.getNextStateId().equals(stateId)) {
					toDelete.add(i);
				}				
			}
			// indices so not change when deleting in reverse ordering
			Collections.sort(toDelete, Collections.reverseOrder());
			Integer index;
			for(int i=0; i< toDelete.size(); i++) {
				index = toDelete.get(i);
				s.deleteArc(index);
			}
		}
	}

	private void writeMap(ObjectOutput out, Mapper<Integer, String> map) throws IOException {
		Integer key;
		String value;
		out.writeInt(map.keySet().size());
		for(Iterator<Integer> it = map.keySet().iterator(); it.hasNext();) {
			key = it.next();
			value = map.getValue(key);
			out.writeInt(key);
			out.writeObject(value);
		}
	}

	@Override
	public void writeExternal(ObjectOutput out) throws IOException {
		out.writeObject(start);
		writeMap(out, isyms);
		writeMap(out, osyms);
		out.writeObject(semiring);
		out.writeInt(states.size());
		State<T> s;
		Arc<T> a;
		for(Iterator<State<T>> it = states.iterator(); it.hasNext();) {
			s = it.next();
			
			out.writeObject(s.getId());
			out.writeObject(s.getFinalWeight().getValue());
			out.writeInt(s.getNumArcs());
			for(Iterator<Arc<T>> itA = s.arcIterator(); itA.hasNext();) {
				a = itA.next();
				out.writeInt(a.getIlabel());
				out.writeInt(a.getOlabel());
				out.writeObject(a.getWeight().getValue());
				out.writeObject(a.getNextStateId());
			}
		}
	}

	private Mapper<Integer, String> readMap(ObjectInput in) throws IOException, ClassNotFoundException {
		Mapper<Integer, String> map = new Mapper<Integer, String>();
		
		int mapSize = in.readInt();
		int key;
		String value;
		for(int i=0; i<mapSize; i++) {
			key = in.readInt();
			value = (String) in.readObject();
			map.put(key, value);
		}
		
		return map;
	}
	
	@SuppressWarnings("unchecked")
	@Override
	public void readExternal(ObjectInput in) throws IOException, ClassNotFoundException {
		start = (String) in.readObject();
		isyms = readMap(in);
		osyms = readMap(in);
		semiring = (Semiring<T>) in.readObject();
		int numStates = in.readInt();

		State<T> s;
		Arc<T> a;
		Weight<T> w;
		int numArcs;
		ssyms = new Mapper<Integer, String>();
		for(int i=0; i<numStates; i++) {
			s = new State<T>();
			s.setId((String) in.readObject());
			w = new Weight<T>();
			w.setValue((T) in.readObject());
			s.setFinalWeight(w);
			s.setSemiring(semiring);
			states.add(s);
			ssyms.put(i, s.getId());
			
			numArcs = in.readInt();
			for(int j=0; j<numArcs; j++) {
				a = new Arc<T>();
				a.setIlabel(in.readInt());
				a.setOlabel(in.readInt());
				w = new Weight<T>();
				w.setValue((T) in.readObject());
				a.setWeight(w);
				a.setNextState((String) in.readObject());
				s.addArc(a);
			}
		}
	}
}

