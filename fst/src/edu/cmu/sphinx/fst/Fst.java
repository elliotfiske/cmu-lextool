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
import java.util.HashMap;

import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 * @param <W>
 */
public class Fst {

    // holds the fst states
    private ArrayList<State> states = new ArrayList<State>();

    // the initial state's id
    private int start;

    // input symbols map
    private HashMap<String, Integer> isyms;

    // output symbols map
    private HashMap<String, Integer> osyms;

    // state symbols map (maps the states' ids to the index in the array)
    // HashMap<key, value>
    // key = state id
    // value = array's index
    private HashMap<Integer, Integer> ssyms = new HashMap<Integer, Integer>();

    // holds the semiring
    private Semiring semiring;

    /**
     * Default constructor
     */
    public Fst() {
    }

    public Fst(Semiring s) {
        this.semiring = s;
    }

    /**
     * @return the initial state
     */
    public int getStartId() {
        return start;
    }

    /**
     * @return the initial state
     */
    public State getStart() {
        return getStateById(start);
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
    // public Mapper<Integer, Integer> getSsyms() {
    // return ssyms;
    // }

    /**
     * @param semiring the semiring to set
     */
    public void setSemiring(Semiring semiring) {
        this.semiring = semiring;
    }

    /**
     * @param stateId the initial state to set
     */
    public void setStart(int stateId) {
        State s = getStateById(stateId);
        if (s == null) {
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
    public void setFinal(int stateId, float w) {
        State s = getStateById(stateId);
        if (s == null) {
            System.err.println("Cannot find state.");
            return;
        }

        s.setFinalWeight(w);
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
    public ArrayList<State> getStates() {
        return states;
    }

    /**
     * Adds a state to the fst
     * 
     * @param s the state to be added
     * @return the state's index
     */
    public int addState(State state) {
        if (ssyms.get(state.getId()) != null) {
            System.err.println("State is already in the fst;");
            return state.getId();
        }
        if (state.getId() < 0) {
            state.setId(states.size());
        }

        this.ssyms.put(state.getId(), states.size());
        this.states.add(state);

        return state.getId();
    }

    /**
     * 
     * @param sIndex the state's index
     * @return the state
     */
    public State getStateById(int stateId) {
        Integer index = ssyms.get(stateId);
        if (index == null) {
            return null;
        }

        return states.get(index);
    }

    /**
     * @return the isyms
     */
    public HashMap<String, Integer> getIsyms() {
        return isyms;
    }

    /**
     * @param isyms the isyms to set
     */
    public void setIsyms(HashMap<String, Integer> isyms) {
        this.isyms = isyms;
    }

    /**
     * @return the osyms
     */
    public HashMap<String, Integer> getOsyms() {
        return osyms;
    }

    /**
     * @param osyms the osyms to set
     */
    public void setOsyms(HashMap<String, Integer> osyms) {
        this.osyms = osyms;
    }

    /**
     * adds a new outgoing arc from a state
     * 
     * @param stateId the state's id
     * @param arc the arc
     */
    public void addArc(int stateId, Arc arc) {
        State s = getStateById(stateId);
        if (s == null) {
            System.err.println("Cannot find state.");
            return;
        }

        s.addArc(arc);
    }

    private void writeStringMap(ObjectOutputStream out,
            HashMap<String, Integer> map) throws IOException {
        out.writeInt(map.values().size());
        for (String key : map.keySet()) {
            Integer value = map.get(key);
            out.writeObject(key);
            out.writeInt(value);
        }
    }

    public void writeFst(ObjectOutputStream out) throws IOException {
        writeStringMap(out, isyms);
        writeStringMap(out, osyms);
        out.writeInt(start);

        out.writeObject(semiring);
        out.writeInt(states.size());

        for (State s : states) {
            out.writeFloat(s.getFinalWeight());
            out.writeInt(s.getId());
        }

        for (State s : states) {
            out.writeInt(s.getNumArcs());
            for (Arc a : s.getArcs()) {
                out.writeInt(a.getIlabel());
                out.writeInt(a.getOlabel());
                out.writeFloat(a.getWeight());
                out.writeInt(a.getNextStateId());
            }
        }
    }

    /**
     * saves binary model to disk
     * 
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

    private static HashMap<String, Integer> readStringMap(ObjectInputStream in)
            throws IOException, ClassNotFoundException {
        HashMap<String, Integer> map = new HashMap<String, Integer>();

        int mapSize = in.readInt();
        for (int i = 0; i < mapSize; i++) {
            String key = (String) in.readObject();
            Integer value = in.readInt();
            map.put(key, value);
        }

        return map;
    }

    private static Fst readFst(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        Fst res = new Fst();
        res.isyms = readStringMap(in);
        res.osyms = readStringMap(in);
        res.start = in.readInt();
        res.semiring = (Semiring) in.readObject();
        int numStates = in.readInt();

        for (int i = 0; i < numStates; i++) {
            float f = in.readFloat();
            if (f == res.semiring.zero()) {
                f = res.semiring.zero();
            } else if (f == res.semiring.one()) {
                f = res.semiring.one();
            }
            State s = new State(f);
            s.setId(in.readInt());
            res.addState(s);
        }

        for (State s1 : res.getStates()) {
            int numArcs = in.readInt();
            for (int j = 0; j < numArcs; j++) {
                Arc a = new Arc();
                a.setIlabel(in.readInt());
                a.setOlabel(in.readInt());
                a.setWeight(in.readFloat());
                a.setNextState(in.readInt());
                s1.addArc(a);
            }
        }

        return res;
    }

    /**
     * Loads a compressed binary model from disk
     * 
     * @param filename the binary model filename
     * @throws IOException
     * @throws ClassNotFoundException
     */
    public static Fst loadModel(String filename) {
        long starttime = GregorianCalendar.getInstance().getTimeInMillis();
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

        System.err
                .println("Load Time: "
                        + (GregorianCalendar.getInstance().getTimeInMillis() - starttime)
                        / 1000.);
        return obj;
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
        if (start != other.start)
            return false;
        if (states == null) {
            if (other.states != null)
                return false;
        } else if (!states.equals(other.states))
            return false;
        if (semiring == null) {
            if (other.semiring != null)
                return false;
        } else if (!semiring.equals(other.semiring))
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
        sb.append("Fst(start=" + start + ", isyms=" + isyms + ", osyms="
                + osyms + ", semiring=" + semiring + ")\n");
        for (State s : states) {
            sb.append("  " + s + "\n");
            for (Arc a : s.getArcs()) {
                sb.append("    " + a + "\n");
            }
        }

        return sb.toString();
    }

    /**
     * Remaps the states
     */
    private void remapStates() {
        // reset symbols table
        ssyms = new HashMap<Integer, Integer>();

        // remap states
        for (int i = 0; i < states.size(); i++) {
            State s = states.get(i);
            ssyms.put(s.getId(), i);
        }
    }

    /**
     * Delete a state
     * 
     * @param i the index of the state
     */
    public void deleteState(int stateId) {
        if (stateId == this.start) {
            System.err.println("Cannot delete start state.");
            return;
        }

        State s = getStateById(stateId);
        if (s == null) {
            System.err.println("Cannot find state.");
            return;
        }

        this.states.remove(s);
        remapStates();

        // delete arc's with nextstate equal to stateid
        ArrayList<Integer> toDelete;
        for (State s1 : states) {
            toDelete = new ArrayList<Integer>();
            for (int j = 0; j < s1.getNumArcs(); j++) {
                Arc a = s1.getArc(j);
                if (a.getNextStateId() == stateId) {
                    toDelete.add(j);
                }
            }
            // indices not change when deleting in reverse ordering
            Collections.sort(toDelete, Collections.reverseOrder());
            for (Integer i : toDelete) {
                s1.deleteArc(i.intValue());
            }
        }
    }
}
