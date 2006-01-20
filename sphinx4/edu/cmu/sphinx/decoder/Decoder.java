/*
 * 
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
package edu.cmu.sphinx.decoder;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;

import edu.cmu.sphinx.decoder.search.SearchManager;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.result.ResultListener;
import edu.cmu.sphinx.util.props.Configurable;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.PropertyType;
import edu.cmu.sphinx.util.props.Registry;
/**
 * The primary decoder class
 */
public class Decoder implements Configurable {
    /**
     * The sphinx property name for the number of features to recognize at
     * once.
     */
    public final static String PROP_FEATURE_BLOCK_SIZE = "featureBlockSize";
    /**
     * The default value of the property PROP_FEATURE_BLOCK_SIZE.
     */
    public final static int PROP_FEATURE_BLOCK_SIZE_DEFAULT = 100000;
    /**
     * The sphinx property name for the name of the search manager to use
     */
    public final static String PROP_SEARCH_MANAGER = "searchManager";
    private String name;
    private SearchManager searchManager;
    private int featureBlockSize;
    
    private List resultListeners = Collections.synchronizedList(new ArrayList());
    
    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#register(java.lang.String,
     *      edu.cmu.sphinx.util.props.Registry)
     */
    public void register(String name, Registry registry)
            throws PropertyException {
        this.name = name;
        registry.register(PROP_FEATURE_BLOCK_SIZE, PropertyType.INT);
        registry.register(PROP_SEARCH_MANAGER, PropertyType.COMPONENT);
    }
    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
     */
    public void newProperties(PropertySheet ps) throws PropertyException {
        featureBlockSize = ps.getInt(PROP_FEATURE_BLOCK_SIZE,
                PROP_FEATURE_BLOCK_SIZE_DEFAULT);
        searchManager = (SearchManager) ps.getComponent(PROP_SEARCH_MANAGER,
                SearchManager.class);
    }
    
    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#getName()
     */
    public String getName() {
        return name;
    }
    
    /**
     * Decode frames until recognition is complete
     * 
     * @param referenceText the reference text (or null)
     * @return a result
     */
    public Result decode(String referenceText) {
        searchManager.startRecognition();
        Result result;
        do {
            result = searchManager.recognize(featureBlockSize);
            if (result != null) {
                result.setReferenceText(referenceText);
                fireResultListeners(result);
            }
        } while (result != null && !result.isFinal());
        searchManager.stopRecognition();
        return result;
    }
    
    /**
     * Allocate resources necessary for decoding
     *
     */
    public void allocate() throws IOException {
        searchManager.allocate();
    }
    
    /**
     * Deallocate resources
     */
    public void deallocate() {
        searchManager.deallocate();
    }
    
    
    /**
     * Adds a result listener to this recognizer. A result listener is called
     * whenever a new result is generated by the recognizer. This method can be
     * called in any state.
     * 
     * @param resultListener
     *            the listener to add
     */
    public void addResultListener(ResultListener resultListener) {
        resultListeners.add(resultListener);
    }
    
    
    /**
     * Removes a previously added result listener. This method can be called in
     * any state.
     * 
     * @param resultListener
     *            the listener to remove
     */
    public void removeResultListener(ResultListener resultListener) {
        resultListeners.remove(resultListener);
    }
    
    /**
     * fires the new result event
     * 
     * @param result the new result
     */
    private void fireResultListeners(Result result) {
        synchronized(resultListeners) {
            for (Iterator i = resultListeners.iterator(); i.hasNext(); ) {
                ResultListener resultListener = (ResultListener) i.next();
                resultListener.newResult(result);
            }
        }
    }

    public SearchManager getSearchManager() {
        return searchManager;
    }
}
