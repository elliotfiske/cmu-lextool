/*
 * Copyright 1999-2003 Carnegie Mellon University.  
 * Portions Copyright 2003 Sun Microsystems, Inc.  
 * Portions Copyright 2003 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.util;

import edu.cmu.sphinx.search.SentenceHMMState;
import edu.cmu.sphinx.search.SentenceHMMStateArc;
import edu.cmu.sphinx.search.LinguistProcessor;
import edu.cmu.sphinx.search.Linguist;
import java.io.PrintStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Set;


/**
 *  A linguist processor that dumps out the sentence hmm in a simple
 *  format. This processor is designed so that it can be easily
 *  extended by replacing the dumpNode and the dumpEdge methods.
 */
public class LinguistDumper implements LinguistProcessor  {

    /**
     * A sphinx property name for the destination of the LinguistDumper
     */
    public final static String PROP_FILENAME 
    		= "edu.cmu.sphinx.util.LinguistDumper.filename";

    private boolean depthFirst = true;

    /**
     * Dumps the sentence hmm in GDL format
     *
     * @param fileName the place to dump the output
     * @param state the initial state of the SentenceHMM
     *
     * @return <code>true</code>  if the file was successfully dumped
     */
    public void process(SphinxProperties props, Linguist linguist) {
        String fileName = props.getString(PROP_FILENAME, getDefaultName());

	try {
	    FileOutputStream fos = new FileOutputStream(fileName);
	    PrintStream out = new PrintStream(fos);
	    dumpSentenceHMM(out, linguist.getInitialState(), props);
	    out.close();
	} catch (FileNotFoundException fnfe) {
	    System.out.println("Can't dump to file " 
		    + fileName + " " + fnfe);
	}
    }

    /**
     * Sets whether the traversal is depth first or breadth first
     *
     * @param depthFirst if true traversal is depth first, otherwise
     * the traversal is breadth first
     */
    protected void setDepthFirst(boolean depthFirst) {
	this.depthFirst = depthFirst;
    }

    /**
     * Retreives the default name for the destination dump. This
     * method is typically overridden by derived classes
     *
     *
     * @return the default name for the file.
     */
    protected String getDefaultName() {
	return "linguistDump.txt";
    }

    /**
     * Called at the start of the dump
     *
     * @param out the output stream.
     */
    protected void startDump(PrintStream out) {
    }

    /**
     * Called at the end of the dump
     *
     * @param out the output stream.
     */
    protected void endDump(PrintStream out) {
    }

    /**
     * Called to dump out a node in the SentenceHMM
     *
     * @param out the output stream.
     * @param state the state to dump
     * @param level the level of the state
     */
    protected void startDumpNode(PrintStream out, 
                                 SentenceHMMState state, int level,
                                 SphinxProperties props) {
    }

    /**
     * Called to dump out a node in the SentenceHMM
     *
     * @param out the output stream.
     * @param state the state to dump
     * @param level the level of the state
     */
    protected void endDumpNode(PrintStream out, SentenceHMMState state, 
	    int level) {
    }

    /**
     * Dumps an arc
     *
     * @param out the output stream.
     * @param from arc leaves this state
     * @param arc the arc to dump
     * @param level the level of the state
     */
    protected void dumpArc(PrintStream out, SentenceHMMState from, 
                           SentenceHMMStateArc arc, int level,
                           SphinxProperties props) {
    }


    /**
     * Dumps the sentence hmm in 
     *
     * @param name out place to dump the output
     * @param state the initial state of the SentenceHMM
     * @param props the SphinxProperties to use
     */
    private void dumpSentenceHMM(PrintStream out, 
                                 SentenceHMMState startingState,
                                 SphinxProperties props) {
	List queue = new LinkedList();
	Set visitedStates = new HashSet();

	startDump(out);
	queue.add(new StateLevel(startingState, 0));

	while (queue.size() > 0) {
	    StateLevel stateLevel = (StateLevel) queue.remove(0);
	    int level = stateLevel.getLevel();
	    SentenceHMMState state = stateLevel.getState();


	    if (!visitedStates.contains(state)) {
		visitedStates.add(state);

		startDumpNode(out, state, level, props);
		SentenceHMMStateArc[] arcs = state.getSuccessorArray(); 


		for (int i = arcs.length - 1; i >= 0; i--) {
		    SentenceHMMState nextState = arcs[i].getNextState();
		    dumpArc(out, state, arcs[i], level, props);
		    if (depthFirst) {
			// if depth first, its a stack
			queue.add(0, new StateLevel(nextState, level + 1));
		    } else {
			queue.add(new StateLevel(nextState, level + 1));
		    }
		}
		endDumpNode(out, state, level);
	    }
	}
	endDump(out);
    }
}


/**
 * A class for bundling together a SentenceHMMState and its level.
 */
class StateLevel {
    private int level;
    private SentenceHMMState state;

    /**
     * Constructs a StateLevel from its primitive components.
     *
     * @param state the state to be bundled in the StateLevel
     * @param level the level of the state
     */
    StateLevel(SentenceHMMState state, int level) {
	this.state = state;
	this.level = level;
    }

    /**
     * Returns the state
     *
     * @return the state
     */
    SentenceHMMState getState() {
	return state;
    }

    /**
     * Returns the level
     *
     * @return the level.
     */
    int getLevel() {
	return level;
    }

    public String toString() {
	return "" + level + " " + state.getSignature() +
	    " 1 " + state.getTypeLabel();
    }
}

