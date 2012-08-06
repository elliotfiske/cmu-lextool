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

package edu.cmu.sphinx.fst.openfst;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.HashMap;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.utils.Utils;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Convert {

    private Convert() {
    }

    public static void export(Fst fst, Semiring semiring, String basename) {
        exportSymbols(fst.getIsyms(), basename + ".input.syms");
        exportSymbols(fst.getOsyms(), basename + ".output.syms");
        exportFst(fst, semiring, basename + ".fst.txt");
    }

    private static void exportFst(Fst fst, Semiring semiring, String filename) {
        FileWriter file;
        try {
            file = new FileWriter(filename);
            PrintWriter out = new PrintWriter(file);

            // print start first
            State start = fst.getStart();
            out.println(start.getId() + "\t"
                    + start.getFinalWeight());

            // print all states
            int numStates = fst.getNumStates();
            for (int i=0; i<numStates; i++) {
                State s = fst.getState(i);
                if (s.getId() != fst.getStart().getId()) {
                    out.println(s.getId() + "\t"
                            + s.getFinalWeight());
                }
            }

            String[] isyms = fst.getIsyms();
            String[] osyms = fst.getOsyms();
            numStates = fst.getNumStates();
            for (int i=0; i<numStates; i++) {
                State s = fst.getState(i);
                int numArcs = s.getNumArcs();
                for (int j=0; i<numArcs; j++) {
                    Arc arc = s.getArc(j);
                    String isym = (isyms != null) ? isyms[arc.getIlabel()] : Integer.toString(arc
                            .getIlabel());
                    String osym = (osyms != null) ? osyms[arc.getOlabel()] : Integer.toString(arc
                            .getOlabel());

                    out.println(s.getId() + "\t"
                            + arc.getNextState().getId()
                            + "\t" + isym + "\t" + osym + "\t"
                            + arc.getWeight());
                }
            }

            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }

    }

    private static void exportSymbols(String[] syms,
            String filename) {
        if (syms == null)
            return;

        try {
            FileWriter file = new FileWriter(filename);
            PrintWriter out = new PrintWriter(file);

            for (int i=0; i<syms.length; i++) {
                String key = syms[i];
                out.println(key + "\t" + i);
            }

            out.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    private static ArrayList<String> importSymbols(String filename) {
        ArrayList<String>  syms = null;

        try {
            FileInputStream fis = new FileInputStream(filename);
            DataInputStream dis = new DataInputStream(fis);
            BufferedReader br = new BufferedReader(new InputStreamReader(dis));
            syms = new ArrayList<String> ();
            String strLine;

            while ((strLine = br.readLine()) != null) {
                String[] tokens = strLine.split("\\t");
                String sym = tokens[0];
                syms.add(sym);
            }

        } catch (IOException e1) {
            return null;
        }

        return syms;
    }

    public static Fst importFloat(String basename, Semiring semiring) {
        Fst fst = new Fst(semiring);

        ArrayList<String> isyms = importSymbols(basename + ".input.syms");
        if (isyms == null) {
            isyms = new ArrayList<String>();
            isyms.add("<eps>");
        }

        ArrayList<String>  osyms = importSymbols(basename
                + ".output.syms");
        if (osyms == null) {
            osyms = new ArrayList<String>();
            osyms.add("<eps>");
        }

        ArrayList<String> ssyms = importSymbols(basename
                + ".states.syms");

        // Parse input
        FileInputStream fis = null;
        try {
            fis = new FileInputStream(basename + ".fst.txt");
        } catch (FileNotFoundException e1) {
            e1.printStackTrace();
            return null;
        }

        DataInputStream dis = new DataInputStream(fis);
        BufferedReader br = new BufferedReader(new InputStreamReader(dis));
        boolean firstLine = true;
        String strLine;
        HashMap<Integer, State> stateMap = new HashMap<Integer, State>();

        try {
            while ((strLine = br.readLine()) != null) {
                String[] tokens = strLine.split("\\t");
                Integer inputStateId;
                if (ssyms == null) {
                    inputStateId = Integer.parseInt(tokens[0]);
                } else {
                    inputStateId = ssyms.indexOf(tokens[0]);
                }
                State inputState = stateMap.get(inputStateId);
                if (inputState == null) {
                    inputState = new State(semiring.zero());
                    fst.addState(inputState);
                    stateMap.put(inputStateId, inputState);
                }

                if (firstLine) {
                    firstLine = false;
                    fst.setStart(inputState);
                }

                if (tokens.length > 2) {
                    Integer nextStateId;
                    if (ssyms == null) {
                        nextStateId = Integer.parseInt(tokens[1]);
                    } else {
                        nextStateId = ssyms.indexOf(tokens[1]);
                    }

                    State nextState = stateMap.get(nextStateId);
                    if (nextState == null) {
                        nextState = new State(semiring.zero());
                        fst.addState(nextState);
                        stateMap.put(nextStateId, nextState);
                    }
                    // Adding arc
                    int iLabel = isyms.indexOf(tokens[2]);
                    if (iLabel < 0) {
                        isyms.add(tokens[2]);
                        iLabel = isyms.size() - 1; 
                    }

                    int oLabel = osyms.indexOf(tokens[3]);
                    if (oLabel < 0) {
                        osyms.add(tokens[3]);
                        oLabel = osyms.size() - 1;
                    }
                    float arcWeight = Float.parseFloat(tokens[4]);
                    Arc arc = new Arc(iLabel, oLabel, arcWeight, nextState);
                    inputState.addArc(arc);
                } else {
                    // This is a final weight
                    float finalWeight = Float.parseFloat(tokens[1]);
                    inputState.setFinalWeight(finalWeight);
                }
            }
            dis.close();
        } catch (IOException e) {
            e.printStackTrace();
            return null;
        }

        fst.setIsyms(Utils.toStringArray(isyms));
        fst.setOsyms(Utils.toStringArray(osyms));

        return fst;
    }
}
