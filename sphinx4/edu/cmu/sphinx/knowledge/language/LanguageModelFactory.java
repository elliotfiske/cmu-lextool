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

package edu.cmu.sphinx.knowledge.language;

import edu.cmu.sphinx.util.SphinxProperties;
import java.io.IOException;



/**
 * This class provides a static method for creating a language model
 * for a particular context.
 */
public class LanguageModelFactory {

    public final static String PROP_PREFIX =
    "edu.cmu.sphinx.knowledge.language.LanguageModelFactory";
    public final static String PROP_CLASS 
			    = PROP_PREFIX + ".languageClass";

    /**
     * Creates a language model based upon the particular context
     *
     * @param context the context
     *
     * @return a language model (or null)
     */
    public static LanguageModel createLanguageModel(String context) {
	String path =  "";
	try {

	   LanguageModel lm = null;
	   SphinxProperties props = 
	       	SphinxProperties.getSphinxProperties(context);
	   path = props.getString(PROP_CLASS, null);

	   if (path != null) {
	       lm = (LanguageModel) Class.forName(path).newInstance();
	       lm.initialize(context);
	   }
	   return lm;
	} catch (ClassNotFoundException fe) {
	    throw new Error("CNFE:Can't create language model " + path, fe);
	} catch (InstantiationException ie) {
	    throw new Error("IE: Can't create language model" + path, ie);
	} catch (IllegalAccessException iea) {
	    throw new Error("IEA: Can't create language model" + path, iea);
	} catch (IOException ioe) {
	    throw new Error("IOE: Can't create language model" + path, ioe);
	} 
    }
}

