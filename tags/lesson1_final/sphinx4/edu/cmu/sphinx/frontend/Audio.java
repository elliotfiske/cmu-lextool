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


package edu.cmu.sphinx.frontend;

import edu.cmu.sphinx.frontend.util.Util;


/**
 * Represents an audio data frame of type double.
 */
public class Audio extends Data implements Cloneable {

    private double[] audioSamples;
    private boolean isSpeech = true;


    /**
     * Constructs an Audio object with the given audio data.
     *
     * @param audioSamples the audio samples for this Audio
     * @param collectTime the time at which the audio data is collected
     * @param firstSampleNumber the position of the first sample in the
     *    original data
     */
    public Audio(double[] audioSamples, long collectTime, 
                 long firstSampleNumber) {
        super(collectTime, firstSampleNumber);
	this.audioSamples = audioSamples;
    }


    /**
     * Constructs an Audio object with the given audio data and Utterance.
     *
     * @param audioSamples the audio samples for this Audio
     * @param utterance the Utterance associated with this Audio
     * @param collectTime the time at which the audio data is collected
     * @param firstSampleNumber the position of the first sample in the
     *    original data
     */
    public Audio(double[] audioSamples, Utterance utterance, 
                 long collectTime, long firstSampleNumber) {
        super(utterance, collectTime, firstSampleNumber);
	this.audioSamples = audioSamples;
    }


    /**
     * Constructs an Audio object with the given Signal.
     *
     * @param signal the Signal this Audio object carries
     * @param collectTime the time this Audio object is created
     * @param firstSampleNumber the position of the first sample in the
     *    original data
     */
    public Audio(Signal signal, long collectTime, long firstSampleNumber) {
        super(signal, collectTime, firstSampleNumber);
        isSpeech = signal.equals(Signal.CONTENT);
    }

    
    /**
     * Returns the audio samples.
     *
     * @return the audio samples
     */
    public double[] getSamples() {
	return audioSamples;
    }


    /**
     * Marks this Audio object as speech or not.
     *
     * @param isSpeech true for speech, false for non-speech
     */
    public void setSpeech(boolean isSpeech) {
        this.isSpeech = isSpeech;
    }


    /**
     * Returns true if this Audio represents speech, false if not.
     * All Audio is speech by default.
     *
     * @return true if this is speech, false otherwise
     */
    public boolean isSpeech() {
        return isSpeech;
    }


    /**
     * Returns a string representation of this Audio.
     * The format of the string is:
     * <pre>audioFrameLength data0 data1 ...</pre>
     *
     * @return a string representation of this Audio
     */
    public String toString() {
	if (audioSamples != null) {
	    return ("Audio: " + Util.doubleArrayToString(audioSamples));
	} else {
	    return ("Audio: " + getSignal().toString());
	}
    }


    /**
     * Returns a duplicate of this Audio object.
     *
     * @return a duplicate of this Audio object
     */
    public Object clone() {
	if (hasContent()) {
	    Audio audio = (Audio) super.clone();
	    double[] newSamples = (double[]) audioSamples.clone();
	    audio.audioSamples = newSamples;
	    return audio;
	} else {
	    Audio audio = (Audio) super.clone();
	    return audio;
	}
    }
}