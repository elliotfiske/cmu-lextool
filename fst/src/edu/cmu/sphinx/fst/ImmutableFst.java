/**
 * 
 */
package edu.cmu.sphinx.fst;

import java.io.BufferedInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.util.Arrays;
import java.util.GregorianCalendar;

import edu.cmu.sphinx.fst.semiring.Semiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class ImmutableFst extends Fst {
    private ImmutableState[] states = null;
    private int numStates;

    private ImmutableFst(int numStates) {
        super(0);
        this.numStates = numStates;
        this.states = new ImmutableState[numStates];
    }

    @Override
    public int getNumStates() {
        return this.numStates;
    }

    /**
     * @return the states
     */

    @Override
    public ImmutableState getState(int index) {
        return states[index];
    }

    public void addState(State state) {
        throw new IllegalArgumentException("You cannot modify an ImmutableFst.");
    }

    public void saveModel(String filename) throws IOException {
        throw new IllegalArgumentException(
                "You cannot serialize an ImmutableFst.");
    }

    protected static ImmutableFst readImmutableFst(ObjectInputStream in)
            throws IOException, ClassNotFoundException {
        String[] is = readStringMap(in);
        String[] os = readStringMap(in);
        int startid = in.readInt();
        Semiring semiring = (Semiring) in.readObject();
        int numStates = in.readInt();
        ImmutableFst res = new ImmutableFst(numStates);
        res.isyms = is;
        res.osyms = os;
        res.semiring = semiring;
        for (int i = 0; i < numStates; i++) {
            int numArcs = in.readInt();
            ImmutableState s = new ImmutableState(numArcs + 1);
            float f = in.readFloat();
            if (f == res.semiring.zero()) {
                f = res.semiring.zero();
            } else if (f == res.semiring.one()) {
                f = res.semiring.one();
            }
            s.setFinalWeight(f);
            s.id = in.readInt();
            res.states[s.getId()] = s;
        }
        res.setStart(res.states[startid]);

        numStates = res.states.length;
        for (int i = 0; i < numStates; i++) {
            ImmutableState s1 = res.states[i];
            for (int j = 0; j < s1.initialNumArcs - 1; j++) {
                Arc a = new Arc();
                a.setIlabel(in.readInt());
                a.setOlabel(in.readInt());
                a.setWeight(in.readFloat());
                a.setNextState(res.states[in.readInt()]);
                s1.setArc(j, a);
            }
        }

        return res;
    }

    public static ImmutableFst loadModel(String filename) {
        long starttime = GregorianCalendar.getInstance().getTimeInMillis();
        ImmutableFst obj;

        try {
            FileInputStream fis = null;
            BufferedInputStream bis = null;
            ObjectInputStream ois = null;
            fis = new FileInputStream(filename);
            bis = new BufferedInputStream(fis);
            ois = new ObjectInputStream(bis);
            obj = readImmutableFst(ois);
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

    @Override
    public void deleteState(State state) {
        throw new IllegalArgumentException("You cannot modify an ImmutableFst.");
    }

    public String toString() {
        StringBuilder sb = new StringBuilder();
        sb.append("Fst(start=" + start + ", isyms=" + isyms + ", osyms="
                + osyms + ", semiring=" + semiring + ")\n");
        int numStates = states.length;
        for (int i = 0; i < numStates; i++) {
            State s = states[i];
            sb.append("  " + s + "\n");
            int numArcs = s.getNumArcs();
            for (int j = 0; j < numArcs; j++) {
                Arc a = s.getArc(j);
                sb.append("    " + a + "\n");
            }
        }

        return sb.toString();
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
        if (getClass() != obj.getClass())
            return false;
        ImmutableFst other = (ImmutableFst) obj;
        if (!Arrays.equals(states, other.states))
            return false;
        if (!super.equals(obj))
            return false;
        return true;
    }

}
