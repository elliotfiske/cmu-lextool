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

package edu.cmu.sphinx.frontend.feature;

import edu.cmu.sphinx.frontend.BaseDataProcessor;
import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.frontend.DataEndSignal;
import edu.cmu.sphinx.frontend.DataProcessingException;
import edu.cmu.sphinx.frontend.DataProcessor;
import edu.cmu.sphinx.frontend.DoubleData;

import edu.cmu.sphinx.util.SphinxProperties;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;


/**
 * Subtract the mean from all the Data objects between a
 * DataStartSignal and a DataEndSignal.
 * It will read in all the Data objects, calculate the mean, 
 * and subtract this mean from all the Data objects. As a result,
 * this process can introduce a significant processing delay because
 * all the Data objects have to be read in before the mean can
 * be calculated. This is fine in batch mode, but in live mode,
 * in which fast response times are needed, one should use the LiveCMN.
 * This process is typically applied to cepstrum objects, in which
 * case it is called cepstral mean normalization (CMN).
 */
public class BatchCMN extends BaseDataProcessor {
    
    private double[] sums;           // array of current sums
    private List cepstraList;
    private int numberDataCepstra;


    /**
     * Initializes this BatchCMN.
     *
     * @param name        the name of this BatchCMN
     * @param frontEnd    the front end this BatchCMN belongs to
     * @param sphinxProps the SphinxProperties to read properties from
     * @param predecessor the DataProcessor from which this normalizer
     *                    obtains Data to normalize
     */
    public void initialize(String name, String frontEnd,
			   SphinxProperties props, DataProcessor predecessor) {
	super.initialize((name == null ? "BatchCMN" : name),
                         frontEnd, props, predecessor);
        sums = null;
        cepstraList = new LinkedList();
    }


    /**
     * Initializes the sums array and clears the cepstra list.
     */
    private void reset() {
        sums = null; // clears the sums array
	cepstraList.clear();
	numberDataCepstra = 0;
    }


    /**
     * Returns the next Data object, which is a normalized cepstrum.
     * Signal objects are returned unmodified.
     *
     * @return the next available Data object, returns null if no
     *         Data object is available
     *
     * @throws DataProcessingException if there is an error processing data
     */
    public Data getData() throws DataProcessingException {

        Data output = null;

        if (cepstraList.size() > 0) {
            output = (Data) cepstraList.remove(0);
        } else {
            reset();
            // read the cepstra of the entire utterance, calculate
            // and apply the cepstral mean
            if (readUtterance() > 0) {
                normalizeList();
                output = (Data) cepstraList.remove(0);//getData();
            }            
        }

        return output;
    }


    /**
     * Reads the cepstra of the entire Utterance into the cepstraList.
     *
     * @return the number cepstra (with Data) read
     *
     * @throws DataProcessingException if an error occurred reading the Data
     */
    private int readUtterance() throws DataProcessingException {

        Data input = null;

        do {
            input = getPredecessor().getData();
            if (input != null) {
                if (input instanceof DoubleData) {
                    numberDataCepstra++;
		    double[] cepstrumData = ((DoubleData) input).getValues();
                    if (sums == null) {
                        sums = new double[cepstrumData.length];
                    } else {
                        if (sums.length != cepstrumData.length) {
                            throw new Error
                                ("Inconsistent cepstrum lengths: sums: " +
                                 sums.length + ", cepstrum: " + 
                                 cepstrumData.length);
                        }
                    }
		    // add the cepstrum data to the sums
                    for (int j = 0; j < cepstrumData.length; j++) {
                        sums[j] += cepstrumData[j];
                    }
                    cepstraList.add(input);

                } else if (input instanceof DataEndSignal) {
                    cepstraList.add(input);
                    break;
                } else { // DataStartSignal or other Signal
                    cepstraList.add(input);
                }
            }
        } while (input != null);

        return numberDataCepstra;
    }
        

    /**
     * Normalizes the list of Data.
     */
    private void normalizeList() {

        // calculate the mean first
        for (int i = 0; i < sums.length; i++) {
            sums[i] /= numberDataCepstra;
        }

        for (Iterator iterator = cepstraList.iterator(); iterator.hasNext();) {
            Data cepstrumObject = (Data) iterator.next();
            if (cepstrumObject instanceof DoubleData) {
                double[] cepstrum = ((DoubleData) cepstrumObject).getValues();
                for (int j = 0; j < cepstrum.length; j++) {
                    cepstrum[j] -= sums[j]; // sums[] is now the means[]
                }
            }
        }
    }
}
