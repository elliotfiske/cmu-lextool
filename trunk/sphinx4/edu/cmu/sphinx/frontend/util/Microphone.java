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


package edu.cmu.sphinx.frontend.util;

import edu.cmu.sphinx.frontend.Audio;
import edu.cmu.sphinx.frontend.AudioSource;
import edu.cmu.sphinx.frontend.DataProcessor;
import edu.cmu.sphinx.frontend.FrontEnd;
import edu.cmu.sphinx.frontend.Signal;
import edu.cmu.sphinx.frontend.Utterance;
import edu.cmu.sphinx.frontend.util.Util;

import edu.cmu.sphinx.util.SphinxProperties;

import java.io.IOException;
import java.io.InputStream;
import java.io.FileOutputStream;
import java.io.File;

import java.util.Collections;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.LinkedList;
import java.util.List;
import java.util.Arrays;
import java.util.Vector;

import javax.sound.sampled.AudioFormat;
import javax.sound.sampled.AudioFileFormat;
import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.AudioInputStream;
import javax.sound.sampled.DataLine;
import javax.sound.sampled.LineEvent;
import javax.sound.sampled.LineListener;
import javax.sound.sampled.LineUnavailableException;
import javax.sound.sampled.TargetDataLine;
import javax.sound.sampled.Line;


/**
 * A Microphone captures audio data from the system's underlying
 * audio input systems. Converts these audio data into Audio
 * objects. The Microphone should be run in a separate thread.
 * When the method <code>startRecording()</code> is called, it will
 * start capturing audio, and stops when <code>stopRecording()</code>
 * is called. An Utterance is created for all the audio captured
 * in between calls to <code>startRecording()</code> and
 * <code>stopRecording()</code>.
 * Calling <code>getAudio()</code> returns the captured audio
 * data as Audio objects.
 */
public class Microphone extends DataProcessor implements AudioSource {

    private final static String PROP_PREFIX = 
        "edu.cmu.sphinx.frontend.util.Microphone.";

    /**
     * Sphinx property that specifies whether or not the microphone
     * will release the audio between utterances.  On certain systems
     * (linux for one), closing and reopening the audio does not work
     * too well.
     */
    public final static String PROP_CLOSE_AUDIO_BETWEEN_UTTERANCES =
	PROP_PREFIX + "closeAudioBetweenUtterances";

    /**
     * The default value for the PROP_CLOSE_AUDIO_BETWEEN_UTTERANCES
     * property
     */
    public final static boolean
	PROP_CLOSE_AUDIO_BETWEEN_UTTERANCES_DEFAULT = true;

    /**
     * The Sphinx property that specifies the amount of time in
     * milliseconds the microphone should sleep in between reading
     * audio. It is recommended to set this property to true
     * on a single CPU system, to boost the performance of
     * live decoding.
     */
    public final static String PROP_SLEEP_BETWEEN_AUDIO =
        PROP_PREFIX + "sleepBetweenAudio";

    /**
     * The default value for PROP_SLEEP_TIME.
     */
    public final static boolean PROP_SLEEP_BETWEEN_AUDIO_DEFAULT = false;


    /**
     * Parameters for audioFormat
     */
    private AudioFormat audioFormat;
    private float sampleRate = 8000f;
    private int sampleSizeInBits = 16;
    private int channels = 1;
    private boolean signed = true;
    private boolean bigEndian = true;

    /**
     * Variables for performing format conversion from a 
     * hardware supported format to the speech recognition audio format
     */
    private boolean doConversion = false;
    private AudioInputStream nativelySupportedStream;

    /**
     * The audio capturing device.
     */
    private TargetDataLine audioLine = null;
    private AudioInputStream audioStream = null;
    private LineListener lineListener = new MicrophoneLineListener();
    private List audioList;
    private Utterance currentUtterance;

    private int frameSizeInBytes;
    private int sleepTime;
    private volatile boolean started = false;
    private volatile boolean recording = false;
    private volatile boolean closed = false;
    private boolean tracing = false;
    private boolean closeAudioBetweenUtterances = true;
    private boolean keepAudioReference = true;
    private boolean sleepBetweenAudio = false;

    private static Logger logger = Logger.getLogger
        ("edu.cmu.sphinx.frontend.Microphone");
    

    /**
     * Constructs a Microphone with the given InputStream.
     *
     * @param name the name of this Microphone
     * @param context the context of this Microphone
     * @param props the SphinxProperties to read properties from
     */
    public Microphone(String name, String context, SphinxProperties props) 
	throws IOException {
        super(name, context);
	setProperties(props);
        audioFormat = new AudioFormat(sampleRate, sampleSizeInBits,
                                      channels, signed, bigEndian);
        audioList = new LinkedList();
    }


    /**
     * Reads the parameters needed from the static SphinxProperties object.
     */
    public void setProperties(SphinxProperties props) {

        sampleRate = props.getInt(FrontEnd.PROP_SAMPLE_RATE,
                                  FrontEnd.PROP_SAMPLE_RATE_DEFAULT);
	closeAudioBetweenUtterances =
	    props.getBoolean(PROP_CLOSE_AUDIO_BETWEEN_UTTERANCES,
                             PROP_CLOSE_AUDIO_BETWEEN_UTTERANCES_DEFAULT);

        SphinxProperties properties = getSphinxProperties();

        frameSizeInBytes = properties.getInt
            (FrontEnd.PROP_BYTES_PER_AUDIO_FRAME,
             FrontEnd.PROP_BYTES_PER_AUDIO_FRAME_DEFAULT);

        if (frameSizeInBytes % 2 == 1) {
            frameSizeInBytes++;
        }

        sampleSizeInBits = getSphinxProperties().getInt
            (FrontEnd.PROP_BITS_PER_SAMPLE, 
             FrontEnd.PROP_BITS_PER_SAMPLE_DEFAULT);

        keepAudioReference = getSphinxProperties().getBoolean
            (FrontEnd.PROP_KEEP_AUDIO_REFERENCE,
             FrontEnd.PROP_KEEP_AUDIO_REFERENCE_DEFAULT);
        
        sleepBetweenAudio = getSphinxProperties().getBoolean
            (PROP_SLEEP_BETWEEN_AUDIO, PROP_SLEEP_BETWEEN_AUDIO_DEFAULT);

        if (sleepBetweenAudio) {
            sleepTime = getSleepTime();
        }
    }

    /**
     * Calculate the sleep time in milliseconds.
     *
     * @return the sleep time in milliseconds
     */
    private int getSleepTime() {
        float samplesPerFrame = frameSizeInBytes/(sampleSizeInBits/8);
        float timePerFrameInSecs = samplesPerFrame/sampleRate;
        return (int) (timePerFrameInSecs * 1000 * 0.8);
    }

    /**
     * Terminates this Microphone. In this version, it currently
     * does nothing.
     */
    public void terminate() {}


    /**
     * Returns the current AudioFormat used by this Microphone.
     *
     * @return the current AudioFormat
     */
    public AudioFormat getAudioFormat() {
        return audioFormat;
    }


    /**
     * Returns the current Utterance.
     *
     * @return the current Utterance
     */
    public Utterance getUtterance() {
        return currentUtterance;
    }


    /**
     * Prints the given message to System.out.
     *
     * @param message the message to print
     */
    private void printMessage(String message) {
	if (tracing) {
	    System.out.println("Microphone: " + message);
	}
    }


    /**
     * This Thread records audio, and caches them in an audio buffer.
     */

    class RecordingThread extends Thread {

        public RecordingThread(String name) {
            super(name);
        }

        /**
         * Implements the run() method of the Thread class.
         * Records audio, and cache them in the audio buffer.
         */
        public void run() {
            
            if (audioLine != null && audioLine.isOpen()) {
                
		if (audioLine.isRunning()) {
		    printMessage("Whoops: line is running");
		}
                audioLine.start();
                printMessage("started recording");

                if (keepAudioReference) {
                    currentUtterance = new Utterance
                        ("Microphone", getContext());
                }

                audioList.add(new Audio(Signal.UTTERANCE_START));
                                
                while (getRecording() && !getClosed()) {
                    printMessage("reading ...");
                    audioList.add(readAudio(currentUtterance));
                    if (sleepBetweenAudio) {
                        try {
                            Thread.sleep(sleepTime);
                        } catch (InterruptedException ie) {
                            ie.printStackTrace();
                        }
                    }
                }

                audioList.add(new Audio(Signal.UTTERANCE_END));
                
                audioLine.stop();
		if (closeAudioBetweenUtterances) {
                    audioLine.close();
                    try {
                        audioStream.close();
                        if (doConversion) {
                            nativelySupportedStream.close();
                        }
                    } catch(IOException e) {
                        logger.warning("IOException closing audio streams");
                    }
		    audioLine = null;
		}
                
                printMessage("stopped recording");
                
            } else {
                printMessage("Unable to open line");
            }
        }
    }

    /**
     * Reads one frame of audio data, and adds it to the given Utterance.
     *
     * @return an Audio object containing the audio data
     */
    private Audio readAudio(Utterance utterance) {
        // Read the next chunk of data from the TargetDataLine.
        byte[] data = new byte[frameSizeInBytes];
        try {
            int numBytesRead = audioStream.read(data, 0, data.length);
            
            if (numBytesRead != frameSizeInBytes) {
                numBytesRead = (numBytesRead % 2 == 0) ?
                    numBytesRead + 2 : numBytesRead + 3;
                
                byte[] shrinked = new byte[numBytesRead];
                System.arraycopy(data, 0, shrinked, 0, numBytesRead);
                data = shrinked;
            }

            printMessage("recorded 1 frame (" + numBytesRead + ") bytes");

        } catch(IOException e) {
            audioLine.stop();
            audioLine = null;
            e.printStackTrace();
            return null;
        }

        if (keepAudioReference) {
            utterance.add(data);
        }

        double[] samples = Util.bytesToSamples
            (data, 0, data.length, sampleSizeInBits/8, signed);
        
        return (new Audio(samples));
    }

    /**
     * Returns a suitable native audio format.
     *
     * @return a suitable native audio format
     */
    private AudioFormat getNativeAudioFormat() {
        // try to do sample rate conversion
        Line.Info[] lineInfos = AudioSystem.getTargetLineInfo
            (new Line.Info(TargetDataLine.class));

        AudioFormat nativeFormat = null;

        // find a usable target line
        for (int i = 0; i < lineInfos.length; i++) {
            
            AudioFormat[] formats = 
                ((TargetDataLine.Info)lineInfos[i]).getFormats();
            
            for (int j = 0; j < formats.length; j++) {
                
                // for now, just accept downsampling, not checking frame
                // size/rate (encoding assumed to be PCM)
                
                AudioFormat format = formats[j];
                if (format.getEncoding() == audioFormat.getEncoding()
                    && format.getChannels() == audioFormat.getChannels()
                    && format.isBigEndian() == audioFormat.isBigEndian()
                    && format.getSampleSizeInBits() == 
                    audioFormat.getSampleSizeInBits()
                    && format.getSampleRate() > audioFormat.getSampleRate()) {
                    nativeFormat = format;
                    break;
                }
            }
            if (nativeFormat != null) {
                //no need to look through remaining lineinfos
                break;
            }
        }
        return nativeFormat;
    }

    /**
     * Opens the audio capturing device so that it will be ready
     * for capturing audio. Attempts to create a converter if the
     * requested audio format is not directly available.
     *
     * @return true if the audio capturing device is opened successfully;
     *     false otherwise
     */
    private boolean open() {
	if (audioLine != null) {
	    return true;
	}

        DataLine.Info info = new DataLine.Info
            (TargetDataLine.class, audioFormat);

        AudioFormat nativeFormat = null;        
        if (!AudioSystem.isLineSupported(info)) {
            logger.warning(audioFormat + " not supported");
            printMessage(audioFormat + " not supported");
            
            nativeFormat = getNativeAudioFormat();
            
            if (nativeFormat == null) {
                logger.severe("couldn't find suitable target " +
                              "audio format for conversion");
                return false;
            } else {
                printMessage("accepting " + nativeFormat + 
                             " as natively supported format");
                info = new DataLine.Info(TargetDataLine.class, nativeFormat);
                doConversion = true;
            }
        } else {
            doConversion = false;
        }


        // Obtain and open the line and stream.
        try {
            audioLine = (TargetDataLine) AudioSystem.getLine(info);
            audioLine.addLineListener(lineListener);
            if (doConversion) {
                try {
                    nativelySupportedStream = new AudioInputStream(audioLine);
                    audioStream = AudioSystem.getAudioInputStream
                        (audioFormat, nativelySupportedStream);
                } catch (IllegalArgumentException e) {
                    logger.severe("couldn't construct converter " +
                                  "from native audio format");
                    audioLine.close();
                    audioLine = null;
                    e.printStackTrace();
                    return false;
                }
            } else {
                audioStream = new AudioInputStream(audioLine);
            }                
            audioLine.open();
            return true;
        } catch (LineUnavailableException ex) {
            audioLine = null;
            ex.printStackTrace();
            return false;
        }
    }


    /**
     * Clears all cached audio data.
     */
    public void clear() {
        audioList = new LinkedList();
    }


    /**
     * Starts recording audio. This method will return only
     * when a START event is received, meaning that this Microphone
     * has started capturing audio.
     *
     * @return true if the recording started successfully; false otherwise
     */
    public synchronized boolean startRecording() {
        if (open()) {
            setRecording(true);
            RecordingThread recorder = new RecordingThread("Microphone");
            recorder.start();
            while (!getStarted()) {
                try {
                    wait();
                } catch (InterruptedException ie) {
                    ie.printStackTrace();
                }
            }
            return true;
        } else {
            return false;
        }
    }


    /**
     * Stops recording audio.
     */
    public synchronized void stopRecording() {
        if (audioLine != null) {
            setRecording(false);
            setStarted(false);
        }
    }

    
    /**
     * Reads and returns the next Audio object from this
     * Microphone, return null if there is no more audio data.
     * All audio data captured in-between <code>startRecording()</code>
     * and <code>stopRecording()</code> is cached in an Utterance
     * object. Calling this method basically returns the next
     * chunk of audio data cached in this Utterance.
     *
     * @return the next Audio or <code>null</code> if none is
     *     available
     *
     * @throws java.io.IOException
     */
    public Audio getAudio() throws IOException {

        getTimer().start();

        Audio output = null;

        do {
            if (audioList.size() > 0) {
                output = (Audio) audioList.remove(0);
            }
        } while (output == null && recording);

        getTimer().stop();

        return output;
    }


    /**
     * Returns true if there is more data in the Microphone.
     * This happens either if getRecording() return true, or if the
     * buffer in the Microphone has a size larger than zero.
     *
     * @return true if there is more data in the Microphone
     */
    public boolean hasMoreData() {
        boolean moreData;
        synchronized (audioList) {
            moreData = (recording || audioList.size() > 0);
        }
        return moreData;
    }


    private boolean getStarted() {
        return started;
    }


    private void setStarted(boolean started) {
        this.started = started;
    }


    /**
     * Returns true if this Microphone is currently
     * in a recording state; false otherwise
     *
     * @return true if recording, false if not recording
     */ 
    public boolean getRecording() {
        return recording;
    }

    
    /**
     * Sets whether this Microphone is in a recording state.
     *
     * @param recording true to set this Microphone
     * in a recording state false to a non-recording state
     */
    private void setRecording(boolean recording) {
        this.recording = recording;
    }


    /**
     * Returns true if this Microphone thread finished running.
     * Normally, this Microphone is run in its own thread. If this
     * method returns true, it means the <code>run()</code> method
     * of the thread is finished running.
     *
     * @return true if this Microphone thread has finished running
     */
    private boolean getClosed() {
        return closed;
    }


    /**
     * Sets whether to terminate the Microphone thread.
     *
     * @param closed true to terminate the Micrphone thread
     */
    private void setClosed(boolean closed) {
        this.closed = closed;
    }


    /**
     * Provides a LineListener for this Microphone
     */
    class MicrophoneLineListener implements LineListener {

        /**
         * Implements update() method of LineListener interface.
         * Responds to the START line event by waking up all the
         * threads that are waiting on the Microphone's monitor.
         *
         * @param event the LineEvent to handle
         */
        public void update(LineEvent event) {
            // System.out.println("MicrophoneLineListener: update " + event);
            if (event.getType().equals(LineEvent.Type.START)) {
                setStarted(true);
                synchronized (Microphone.this) {
                    Microphone.this.notifyAll();
                }
            }
        }
    }
}
