/*
 * Copyright 2004 Carnegie Mellon University.  
 * Portions Copyright 2004 Sun Microsystems, Inc.  
 * Portions Copyright 2004 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 * @see FrontEnd
 */


package edu.cmu.sphinx.frontend;

import edu.cmu.sphinx.util.SphinxProperties;


/**
 * A processor that performs a signal processing function.
 * Since a DataProcessor usually belongs to a particular front end
 * pipeline, you can name the pipeline it belongs to in the
 * {@link #initialize(String,String,SphinxProperties,DataProcessor) initialize}
 * method. (Note, however, that it is
 * not always the case that a DataProcessor belongs to a particular
 * pipeline. For example, the
 * {@link edu.cmu.sphinx.frontend.util.Microphone Microphone} class
 * is a DataProcessor, but it usually does not belong to any particular
 * pipeline.
 * <p>
 * Each DataProcessor usually have a predecessor as well. This is
 * the previous DataProcessor in the pipeline. Again, not all
 * DataProcessors have predecessors.
 * <p>
 * Calling {@link #getData() getData} will return the processed Data object.
 */
public interface DataProcessor {

    /**
     * Initializes this DataProcessor.
     *
     * @param name the name of this DataProcessor
     * @param frontEndName the front end this DataProcessor belongs to
     * @param sphinxProperties the SphinxProperties to use
     * @param predecessor the predecessor of this DataProcessor
     */
    public void initialize(String name, String frontEndName,
                           SphinxProperties sphinxProperties,
                           DataProcessor predecessor);
    
    /**
     * Returns the processed Data output.
     *
     * @return an Data object that has been processed by this DataProcessor 
     *
     * @throws DataProcessingException if a data processor error occurs
     */
    public abstract Data getData() throws DataProcessingException;

    /**
     * Returns the name of this DataProcessor.
     *
     * @return the name of this DataProcessor
     */
    public String getName();

    /**
     * Returns the predecessor DataProcessor.
     *
     * @return the predecessor
     */
    public DataProcessor getPredecessor();

    /**
     * Sets the predecessor DataProcessor. This method allows dynamic
     * reconfiguration of the front end.
     *
     * @param predecessor the new predecessor of this DataProcessor
     */
    public void setPredecessor(DataProcessor predecessor);
}
