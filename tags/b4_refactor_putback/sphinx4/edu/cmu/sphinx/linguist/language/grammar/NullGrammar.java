/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.linguist.language.grammar;
import java.io.IOException;
/**
 * Defines an empty grammar
 *  
 */
public class NullGrammar extends Grammar {
    /**
     * Creates the grammar from the language model.
     * 
     * @return the initial grammar node
     */
    protected GrammarNode createGrammar() throws IOException,
            NoSuchMethodException {
        return null;
    }
}
