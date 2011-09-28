/*
 * Copyright 1999-2004 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.demo.aligner;


public interface AudioAlignerInterface {
	
	// Allow dynamic audio change
	// returns true if change succeeded
	public boolean setAudio(String pathToAudio);
	
	// Allow dynamic transcription change
	// returns false if change failed
	public boolean setText(String text);
	
	// Allow dynamic grammar-type modification
	public boolean newGrammarType(String grammarType);
	
	// optimize values for aligner configuration
	void optimize();
	
	// align audio and return alignment result
	public String align() throws Exception;
	
}
