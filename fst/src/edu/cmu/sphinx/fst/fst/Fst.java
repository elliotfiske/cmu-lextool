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

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.util.ArrayList;
import java.util.Collections;
import java.util.GregorianCalendar;
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Mapper;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 * @param <W>
 */
public class Fst {
	
	// holds the fst states
	private ArrayList<State> states = new ArrayList<State>();
	
	// the initial state
	private String start;

	// input symbols map
	private Mapper<Integer, String> isyms;
	
	// output symbols map
	private Mapper<Integer, String> osyms;
	
	// state symbols map (maps the states' ids to the index in the array)
	private Mapper<Integer, String> ssyms = new Mapper<Integer, String>();
	
	// holds the semiring
	private Semiring semiring;

	/**
	 *  Default constructor
	 */
	public Fst() {}

	public Fst(Semiring s) {
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
	public State getStart() {
		return states.get(ssyms.getKey(start));
	}
	
	
	/**
	 * @return the semiring
	 */
	public Semiring getSemiring() {
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
	public void setSemiring(Semiring semiring) {
		this.semiring = semiring;
	}

	/**
	 * @param stateId the initial state to set
	 */
	public void setStart(String stateId) {
		if(ssyms.getKey(stateId) == null) {
			System.err.println("Cannot find state.");
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
	public void setFinal(String stateId, double w) {
		Integer index = ssyms.getKey(stateId);
		if(index == null) {
			System.err.println("Cannot find state.");
			return;
		}

		this.states.get(index.intValue()).setFinalWeight(w);
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
	public Iterator<State> stateIterator() {
		return states.iterator();
	}

	
	/**
	 * Adds a state to the fst
	 * 
	 * @param s the state to be added
	 * @return the state's index 
	 */
	public String addState(State state) {
		state.setSemiring(this.semiring);
		if(ssyms.getKey(state.getId()) != null) {
			System.err.println("State is already in the fst;");
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
	public State getStateById(String stateId) {
		Integer index = ssyms.getKey(stateId);
		if(index == null) {
			return null;
		}

		return this.states.get(index.intValue());
	}

	public State getStateByIndex(int index) {
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
	public void addArc(String stateId, Arc arc) {
		Integer index = ssyms.getKey(stateId);
		if(index == null) {
			System.err.println("Cannot find state.");
			return;
		}

		State s = states.get(index.intValue());
		s.addArc(arc);
	}
	
	private void writeMap(ObjectOutputStream out, Mapper<Integer, String> map) throws IOException {
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

	public void writeFst(ObjectOutputStream out) throws IOException {
		writeMap(out, isyms);
		writeMap(out, osyms);
		writeMap(out, ssyms);
		out.writeInt(ssyms.getKey(start));
		
		out.writeObject(semiring);
		out.writeInt(states.size());
		State s;
		Arc a;
		
		for(Iterator<State> it = states.iterator(); it.hasNext();) {
			s = it.next();
			out.writeDouble(s.getFinalWeight());
			out.writeInt(ssyms.getKey(s.getId()));
		}
		
		for(Iterator<State> it = states.iterator(); it.hasNext();) {
			s = it.next();
			out.writeInt(s.getNumArcs());
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				a = itA.next();
				out.writeInt(a.getIlabel());
				out.writeInt(a.getOlabel());
				out.writeDouble(a.getWeight());
				out.writeInt(ssyms.getKey(a.getNextStateId()));
			}
		}
	}
	
	/**
	 *  saves binary model to disk
	 * @param filename the binary model filename
	 * @throws IOException
	 */
	public void saveModel(String filename) throws IOException {
		FileOutputStream fos = new FileOutputStream(filename);
		BufferedOutputStream bos = new BufferedOutputStream(fos);
		ObjectOutputStream oos = new ObjectOutputStream(bos);
		writeFst(oos);
		oos.flush();
        oos.close();
        bos.close();
        fos.close();
	}
	
	private static Mapper<Integer, String> readMap(ObjectInputStream in) throws IOException, ClassNotFoundException {
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

	private static Fst readFst(ObjectInputStream in) throws IOException, ClassNotFoundException {
		Fst res = new Fst();
		res.isyms = readMap(in);
		res.osyms = readMap(in);
		res.ssyms = readMap(in);
		res.start = res.ssyms.getValue(in.readInt());
		res.semiring = (Semiring) in.readObject();
		int numStates = in.readInt();

		State s;
		Arc a;
		int numArcs;
		for(int i=0; i<numStates; i++) {
			s = new State(in.readDouble());
			s.setId(res.ssyms.getValue(in.readInt()));
			s.setSemiring(res.semiring);
			res.states.add(s);
		}
			
		for(Iterator<State> it = res.stateIterator(); it.hasNext();) {
			s = it.next();
			numArcs = in.readInt();
			for(int j=0; j<numArcs; j++) {
				a = new Arc();
				a.setIlabel(in.readInt());
				a.setOlabel(in.readInt());
				a.setWeight(in.readDouble());
				a.setNextState(res.ssyms.getValue(in.readInt()));
				s.addArc(a);
			}
		}
		
		return res;
	}
	
	/**
	 * Loads a compressed binary model from disk
	 * @param filename the binary model filename
	 * @throws IOException
	 * @throws ClassNotFoundException 
	 */
	public static Fst loadModel(String filename) {
		long start =GregorianCalendar.getInstance().getTimeInMillis();
		Fst obj;
        
	    try {
			FileInputStream fis = null;
			BufferedInputStream bis = null;
		    ObjectInputStream ois = null;
	    	fis = new FileInputStream(filename);
	    	bis = new BufferedInputStream(fis);
	        ois = new ObjectInputStream(bis);
	        obj = readFst(ois);
	        ois.close();
	        bis.close();
	        fis.close();
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return null;
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			return null;
		}

		System.err.println("Load Time: "+ (GregorianCalendar.getInstance().getTimeInMillis() - start)/1000.);
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
		Fst other = (Fst) obj;
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
				State s;
				State others;
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
		State s;
		Arc a;
		for(Iterator<State> itS = states.iterator(); itS.hasNext();) {
			s=itS.next();
			sb.append("  " +s+"\n");
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				a = itA.next();
				sb.append("    " +a+"\n");
			}
		}

		return sb.toString();
	}

	/**
	 * Remaps the states
	 */
	private void remapStates() {
		// reset symbols table
		ssyms = new Mapper<Integer, String>();
		
		//remap states
		for(int i=0; i<states.size(); i++) {
			State s = states.get(i);
			ssyms.put(i, s.getId());
		}
	}

	/**
	 * Delete a state
	 * @param i the index of the state
	 */
	public void deleteState(String stateId) {
		if(stateId == this.start) {
			System.err.println("Cannot delete start state.");
			return;
		}

		Integer index = ssyms.getKey(stateId);
		if(index == null) {
			System.err.println("Cannot find state.");
			return;
		}
		
		this.states.remove(index.intValue());
		remapStates();
		
		// delete arc's with nextstate equal to stateid
		State s;
		Arc arc;
		int numA;
		ArrayList<Integer> toDelete;
		for(Iterator<State> itState = this.states.iterator(); itState.hasNext();) {
			s = itState.next();
			toDelete = new ArrayList<Integer>();
			numA = s.getNumArcs();
			for(int j=0; j< numA; j++) {
				arc = s.getArc(j);
				if(arc.getNextStateId().equals(stateId)) {
					toDelete.add(j);
				}				
			}
			// indices not change when deleting in reverse ordering
			Collections.sort(toDelete, Collections.reverseOrder());
			for(Iterator<Integer> it = toDelete.iterator(); it.hasNext();) {
				index = it.next();
				s.deleteArc(index.intValue());
			}
		}
	}

}

