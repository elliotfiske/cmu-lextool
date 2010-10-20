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


package edu.cmu.sphinx.frontend.endpoint;

import java.util.LinkedList;
import java.util.List;

import edu.cmu.sphinx.frontend.BaseDataProcessor;
import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.frontend.DataEndSignal;
import edu.cmu.sphinx.frontend.DataProcessingException;
import edu.cmu.sphinx.frontend.DataProcessor;
import edu.cmu.sphinx.frontend.DataStartSignal;
import edu.cmu.sphinx.frontend.DoubleData;
import edu.cmu.sphinx.frontend.Signal;
import edu.cmu.sphinx.util.SphinxProperties;


/**
 * Given a sequence of Data, filters out the non-speech regions.
 * The sequence of Data should have the speech and non-speech regions
 * marked out by the SpeechStartSignal and SpeechEndSignal, using
 * and endpointer.
 *
 * A sequence of Data for an Utterance should look like:
 *
 * <pre>
 * DataStartSignal (non-speech Data)
 * SpeechStartSignal (speech Data) SpeechEndSignal (non-speech Data)
 * DataEndSignal
 * </pre>
 * or
 * <pre>
 * DataStartSignal (non-speech Data)
 * SpeechStartSignal (speech Data) SpeechEndSignal (non-speech Data)
 * SpeechStartSignal (speech Data) SpeechEndSignal (non-speech Data)
 * ...
 * DataEndSignal
 * </pre>
 * In the first case, where there is only one speech region, the
 * first non-speech region will be removed, and the SpeechStartSignal
 * Signal will be removed. The ending SpeechEndSignal and non-speech
 * region will be removed as well.
 *
 * <p>The second case is a little more complicated. If the SphinxProperty
 * <pre>
 * edu.cmu.sphinx.frontend.DataFilter.mergeSpeechSegments </pre>
 * is set to true (the default),
 * all the Data from the first SpeechStartSignal to the last SpeechEndSignal
 * will be considered as one Utterance, and enclosed by a pair of
 * DataStartSignal and DataEndSignal. The first and last non-speech
 * regions, as well as all SpeechStartSignal and SpeechEndSignal,
 * will obviously be removed. This gives:
 * <pre>
 * DataStartSignal
 * (speech Data) (non-speech Data)
 * (speech Data) (non-speech Data)
 * ...
 * DataEndSignal
 * </pre>
 *
 * <p>On the other hand, if <code>mergeSpeechSegments</code> is set to
 * false, then each:
 * <pre>
 * SpeechStartSignal (speech Data) SpeechEndSignal (non-speech Data)
 * </pre>
 * will become:
 * <pre>
 * DataStartSignal (speech Data) DataEndSignal
 * </pre>
 * that is, the SpeechStartSignal replaced by DataStartSignal, 
 * the SpeechEndSignal replaced by DataEndSignal, and the non-speech
 * region removed. Also, the first DataStartSignal and last
 * DataEndSignal in the original stream will be removed as well.
 * This will give:
 * <pre>
 * DataStartSignal (speech Data) DataEndSignal
 * DataStartSignal (speech Data) DataEndSignal
 * ...
 * </pre>
 */
public class NonSpeechDataFilter extends BaseDataProcessor {

    private static final String PROP_PREFIX 
        = "edu.cmu.sphinx.frontend.endpoint.NonSpeechDataFilter.";


    /**
     * The SphinxProperty that controls whether to merge discontiguous
     * speech segments in an utterance.
     */
    public static final String PROP_MERGE_SPEECH_SEGMENTS
        = PROP_PREFIX + "mergeSpeechSegments";


    /**
     * The default value for PROP_MERGE_SPEECH_SEGMENTS.
     */
    public static final boolean PROP_MERGE_SPEECH_SEGMENTS_DEFAULT = false;


    /**
     * Controls whether to merge multiple speech segments within an
     * Utterance to one big speech segment, with the boundaries being
     * the start of the first speech segment, and the end of the
     * last speech segment.
     */
    private boolean mergeSpeechSegments;
    private boolean discardMode;
    private boolean inSpeech;

    private List inputBuffer;
    private List outputQueue;

    /**
     * The number of samples in a speech segment, used to calculate
     * the duration of the speech segment.
     */
    private int numberSpeechSamples;
    private int sampleRate;


    /**
     * Constructs an NonSpeechDataFilter with the given name, context,
     * and DataSource predecessor.
     *
     * @param name        the name of this NonSpeechDataFilter, if this is
     *                    null, the name "NonSpeechDataFilter" will be given
     * @param frontEnd    the front end this NonSpeechDataFilter is in
     * @param props       the SphinxProperties to read properties from
     * @param predecessor the DataProcessor where this NonSpeechDataFilter
     *                    gets Data from
     */
    public void initialize(String name, String frontEnd,
                           SphinxProperties props, DataProcessor predecessor) {
        super.initialize((name == null ? "NonSpeechDataFilter" : name),
                         frontEnd, props, predecessor);
	
        this.mergeSpeechSegments = props.getBoolean
	    (getName(), PROP_MERGE_SPEECH_SEGMENTS,
             PROP_MERGE_SPEECH_SEGMENTS_DEFAULT);
        
        this.discardMode = true;
        this.inSpeech = false;
        this.inputBuffer = new LinkedList();
        this.outputQueue = new LinkedList();
    }


    /**
     * Prints out a message to System.out.
     */
    private void message(String message) {
        System.out.println("NonSpeechDataFilter: " + message);
    }


    /**
     * Returns the next Data or Signal.
     *
     * @return the next Data, or null if no Data is available
     *
     * @throws DataProcessingException if a data processing error occurs
     */
    public Data getData() throws DataProcessingException {

        if (outputQueue.size() == 0) {
            Data audio = readData();
            
            getTimer().start();
            
            if (audio != null) {
                if (!mergeSpeechSegments) {
                    audio = handleNonMergingData(audio);
                } else {
                    audio = handleMergingData(audio);
                }
            }
            outputQueue.add(audio);
            
            getTimer().stop();
        }

        if (outputQueue.size() > 0) {
            return (Data) outputQueue.remove(0);
        } else {
            return null;
        }
    }


    /**
     * Handles the given Data in the case when mergeSpeechSegment
     * is true.
     *
     * @param audio the Data object to handle
     *
     * @throws DataProcessingException if a data processor error occurs
     */
    private Data handleMergingData(Data audio) throws DataProcessingException {
        Data next = audio;

        if (audio instanceof DataStartSignal) {
            
            // Read (and discard) all the Data from DataStartSignal until
            // we hit a SpeechStartSignal. The SpeechStartSignal is discarded.
            List audioList = readUntilSpeechStartOrDataEnd();
            Data last = (Data) audioList.get(audioList.size() - 1);
            if (last != null) {
                if (last instanceof DataEndSignal) {
                    outputQueue.add(audio);
                    next = last;
                }
            }
        } else if (audio instanceof SpeechEndSignal) {
            // read (and discard) all the Data from SpeechEndSignal
            // until we hit a DataEndSignal
            List audioList = readUntilSpeechStartOrDataEnd();
            Data last = (Data) audioList.get(audioList.size() - 1);
            if (last != null) {
                if (last instanceof SpeechStartSignal) {
                    // first remove the SpeechStartSignal, then add
                    // all the Data to the inputBuffer
                    
                    audioList.remove(last);
                    inputBuffer.addAll(audioList);                    
                    next = readData();
                        
                } else if (last instanceof DataEndSignal) {
                    // System.out.println("Last is DataEndSignal");
                    next = last;
                }
            }
        }
        return next;
    }


    /**
     * Handles the given Data in the case when mergeSpeechSegment
     * is false.
     *
     * @param audio the Data object to handle
     *
     * @throws DataProcessingException if a data processor error occurs
     */
    private Data handleNonMergingData(Data audio) throws 
        DataProcessingException {
        Data next = audio;
        if (audio != null) {
            if (audio instanceof SpeechStartSignal) {

                numberSpeechSamples = 0;

                if (inSpeech) {
                    // Normally, we should not be encounter a SpeechStartSignal
                    // if we are inSpeech. This is error-handling code.
                    message("ALERT: getting a SpeechStartSignal while "+
                            "in speech, removing it.");
                    do {
                        next = readData();
                    } while (next != null &&
                             next instanceof SpeechStartSignal);
                    if (next != null) {
                        next = handleNonMergingData(next);
                    }
                } else {
                    // if we hit a SpeechStartSignal, we will stop discarding
                    // Data, and return an DataStartSignal instead
                    inSpeech = true;
                    discardMode = false;
                    next = new DataStartSignal(((Signal) audio).getTime());
                }
            } else if (audio instanceof SpeechEndSignal) {
                if (!inSpeech) {
                    // Normally, we should not get a SpeechEndSignal
                    // if we are not inSpeech. This is error-handling code.
                    message("ALERT: getting a SpeechEndSignal while "+
                            "not in speech, removing it.");
                    do {
                        next = readData();
                    } while (next != null &&
                             next instanceof SpeechEndSignal);
                    if (next != null) {
                        next = handleNonMergingData(next);
                    }
                } else {
                    // if we hit a SpeechEndSignal, we will start
                    // discarding Data, and return a DataEndSignal instead
                    inSpeech = false;
                    discardMode = true;
                    next = new DataEndSignal
                        (getDuration(), ((Signal) audio).getTime());
                }
            } else if (discardMode) {
                while (next != null && 
                       !(next instanceof SpeechStartSignal) &&
                       !(next instanceof SpeechEndSignal)) {
                    next = readData();
                }
                next = handleNonMergingData(next);
            } else if (audio instanceof DoubleData) {
                DoubleData realData = (DoubleData) audio;
                numberSpeechSamples += realData.getValues().length;
                sampleRate = realData.getSampleRate();
            }
        }
        return next;
    }


    /**
     * Returns the duration of the current speech segment.
     *
     * @return the duration of the current speech segment
     */
    private long getDuration() {
        return (long)
            (((double)numberSpeechSamples/(double)sampleRate) * 1000.0);
    }


    /**
     * Returns the next Data, either from the inputBuffer or the
     * predecessor.
     *
     * @return the next available Data
     *
     * @throws DataProcessingException if a data processor error occurs
     */
    private Data readData() throws DataProcessingException {
        Data audio = null;
        if (inputBuffer.size() > 0) {
            audio = (Data) inputBuffer.remove(0);
        } else {
            audio = getPredecessor().getData();
        }
        return audio;
    }


    /**
     * Read until we hit a SpeechStartSignal or DataEndSignal.
     *
     * @return a list of all the Data read,
     *         including the SpeechStartSignal or DataEndSignal
     */
    private List readUntilSpeechStartOrDataEnd() throws 
        DataProcessingException {
        List audioList = new LinkedList();
        Data audio = null;
        do {
            audio = readData();
            if (audio != null) {
                audioList.add(audio);
            }
        } while (audio != null &&
                 !(audio instanceof SpeechStartSignal) &&
                 !(audio instanceof DataEndSignal));
        return audioList;
    }
}