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
import java.util.Arrays;
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
    private ArrayList<State> states = null;

    // the initial state's id
    protected State start;

    // input symbols map
    protected String[] isyms;

    // output symbols map
    protected String[] osyms;

    // holds the semiring
    protected Semiring semiring;

    /**
     * Default constructor
     */
    public Fst() {
        states = new ArrayList<State>();
    }

    public Fst(int numStates) {
        if (numStates > 0) {
            states = new ArrayList<State>(numStates);
        }
    }

    public Fst(Semiring s) {
        this();
        this.semiring = s;
    }

    /**
     * @return the initial state
     */
    public State getStart() {
        return start;
    }

    /**
     * @return the semiring
     */
    public Semiring getSemiring() {
        return semiring;
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
    public void setStart(State start) {
        this.start = start;
    }

    /**
     * 
     * @return the number of states in the fst
     */
    public int getNumStates() {
        return this.states.size();
    }

    public State getState(int index) {
        return states.get(index);
    }

    /**
     * Adds a state to the fst
     * 
     * @param s the state to be added
     * @return the state's index
     */
    public void addState(State state) {
        this.states.add(state);
        state.id = states.size() - 1;
    }

    /**
     * @return the isyms
     */
    public String[] getIsyms() {
        return isyms;
    }

    /**
     * @param isyms the isyms to set
     */
    public void setIsyms(String[] isyms) {
        this.isyms = isyms;
    }

    /**
     * @return the osyms
     */
    public String[] getOsyms() {
        return osyms;
    }

    /**
     * @param osyms the osyms to set
     */
    public void setOsyms(String[] osyms) {
        this.osyms = osyms;
    }

    private void writeStringMap(ObjectOutputStream out, String[] map)
            throws IOException {
        out.writeInt(map.length);
        for (int i = 0; i < map.length; i++) {
            out.writeObject(map[i]);
        }
    }

    public void writeFst(ObjectOutputStream out) throws IOException {
        writeStringMap(out, isyms);
        writeStringMap(out, osyms);
        out.writeInt(states.indexOf(start));

        out.writeObject(semiring);
        out.writeInt(states.size());

        HashMap<State, Integer> stateMap = new HashMap<State, Integer>(
                states.size(), 1.f);
        for (int i = 0; i < states.size(); i++) {
            State s = states.get(i);
            out.writeInt(s.getNumArcs());
            out.writeFloat(s.getFinalWeight());
            out.writeInt(s.getId());
            stateMap.put(s, i);
        }

        int numStates = states.size();
        for (int i = 0; i < numStates; i++) {
            State s = states.get(i);
            int numArcs = s.getNumArcs();
            for (int j = 0; j < numArcs; j++) {
                Arc a = s.getArc(j);
                out.writeInt(a.getIlabel());
                out.writeInt(a.getOlabel());
                out.writeFloat(a.getWeight());
                out.writeInt(stateMap.get(a.getNextState()));
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

    protected static String[] readStringMap(ObjectInputStream in)
            throws IOException, ClassNotFoundException {

        int mapSize = in.readInt();
        String[] map = new String[mapSize];
        for (int i = 0; i < mapSize; i++) {
            String sym = (String) in.readObject();
            map[i] = sym;
        }

        return map;
    }

    protected static Fst readFst(ObjectInputStream in) throws IOException,
            ClassNotFoundException {
        String[] is = readStringMap(in);
        String[] os = readStringMap(in);
        int startid = in.readInt();
        Semiring semiring = (Semiring) in.readObject();
        int numStates = in.readInt();
        Fst res = new Fst(numStates);
        res.isyms = is;
        res.osyms = os;
        res.semiring = semiring;
        for (int i = 0; i < numStates; i++) {
            int numArcs = in.readInt();
            State s = new State(numArcs + 1);
            float f = in.readFloat();
            if (f == res.semiring.zero()) {
                f = res.semiring.zero();
            } else if (f == res.semiring.one()) {
                f = res.semiring.one();
            }
            s.setFinalWeight(f);
            s.id = in.readInt();
            res.states.add(s);
        }
        res.setStart(res.states.get(startid));

        numStates = res.getNumStates();
        for (int i = 0; i < numStates; i++) {
            State s1 = res.getState(i);
            for (int j = 0; j < s1.initialNumArcs - 1; j++) {
                Arc a = new Arc();
                a.setIlabel(in.readInt());
                a.setOlabel(in.readInt());
                a.setWeight(in.readFloat());
                a.setNextState(res.states.get(in.readInt()));
                s1.addArc(a);
            }
        }

        return res;
    }

    public int getNumArcs() {
        int numArcs = 0;
        for (int i = 0; i < states.size(); i++) {
            numArcs += states.get(i).getNumArcs();
        }

        return numArcs;
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
        if (!Arrays.equals(isyms, other.isyms))
            return false;
        if (!Arrays.equals(osyms, other.osyms))
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
        // for (State s : states) {
        int numStates = states.size();
        for (int i = 0; i < numStates; i++) {
            State s = states.get(i);
            sb.append("  " + s + "\n");
            int numArcs = s.getNumArcs();
            for (int j = 0; j < numArcs; j++) {
                Arc a = s.getArc(j);
                sb.append("    " + a + "\n");
            }
        }

        return sb.toString();
    }

    /**
     * Delete a state
     * 
     * @param i the index of the state
     */
    public void deleteState(State state) {
        if (state.getId() == this.start.getId()) {
            System.err.println("Cannot delete start state.");
            return;
        }

        this.states.remove(state);

        // delete arc's with nextstate equal to stateid
        ArrayList<Integer> toDelete;
        // for (State s1 : states) {
        int numStates = states.size();
        for (int i = 0; i < numStates; i++) {
            State s1 = states.get(i);

            toDelete = new ArrayList<Integer>();
            int numArcs = s1.getNumArcs();
            for (int j = 0; j < numArcs; j++) {
                Arc a = s1.getArc(j);
                if (a.getNextState().equals(state)) {
                    toDelete.add(j);
                }
            }
            // indices not change when deleting in reverse ordering
            Object[] toDeleteArray = toDelete.toArray();
            Arrays.sort(toDeleteArray);
            for (int j = toDelete.size() - 1; j >= 0; j--) {
                Integer index = (Integer) toDeleteArray[j];
                s1.deleteArc(index.intValue());
            }
        }
    }

    public void remapStateIds() {
        int numStates = states.size();
        for(int i=0; i < numStates; i++) {
            states.get(i).id = i;
        }
        
    }
}
