/*
 * Copyright 2013 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 */

package edu.cmu.sphinx.api;

import java.net.URL;

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;


/**
 * Speech recognizer that works with audio resources.
 *
 * @see LiveSpeechRecognizer live speech recognizer
 */
public class BatchSpeechRecognizer extends AbstractSpeechRecognizer {

    private final AudioFileDataSource fileDataSource;

    /**
     * Constructs new batch recognizer object.
     *
     * @param configuraiton basic recognizer configuration
     */
    public BatchSpeechRecognizer(Configuration configuration) {
        super(configuration);
        fileDataSource = configurer.getInstance(AudioFileDataSource.class);
    }

    /**
     * Starts recognition process.
     *
     * Starts recognition process and optionally clears previous data.
     *
     * @param clear clear cached microphone data
     * @see         BatchSpeechRecognizer#stopRecognition()
     */
    public void startRecognition(URL resourceUrl) {
        recognizer.allocate();
        configurer.setSpeechSource(resourceUrl);
    }

    /**
     * Stops recognition process.
     *
     * Recognition process is paused until the next call to startRecognition.
     *
     * @see BatchSpeechRecognizer#startRecognition(boolean)
     */
    public void stopRecognition() {
        recognizer.deallocate();
    }
}
