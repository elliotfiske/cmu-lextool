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
import edu.cmu.sphinx.frontend.Signal;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;


/**
 * A BatchFileAudioSource takes a file (called batch file onwards)
 * that contains a list of audio files,
 * and converts the audio data in each of the audio files into
 * Audio objects. One would obtain the Audio objects using
 * the <code>getAudio()</code> method. This class uses the StreamAudioSource
 * class. In fact, it converts each audio file in the batch file into
 * an InputStream, and sets it to the InputStream of StreamAudioSource.
 * Its getAudio() method then calls StreamAudioSource.getAudio().
 * The only difference is that BatchFileAudiosource
 * takes a batch file, whereas StreamAudioSource takes an InputStream.
 *
 * The format of the batch file would look like: <pre>
 * /home/user1/data/music.au
 * /home/user1/data/talking.au
 * ...
 * </pre>
 *
 * @see StreamAudioSource
 */
public class BatchFileAudioSource extends DataProcessor implements
AudioSource {

    private BufferedReader reader;
    private StreamAudioSource streamAudioSource = null;


    /**
     * Constructs a BatchFileAudioSource with the given name,
     * context and batch file.
     *
     * @param name the name of this BatchFileAudioSource
     * @param context the context of this BatchFileAudioSource
     * @param batchFile contains a list of the audio files
     *
     * @throws java.io.IOException if error opening the batch file
     */
    public BatchFileAudioSource(String name, String context,
                                String batchFile) throws
    IOException {
        super(name, context);
        reader = new BufferedReader(new FileReader(batchFile));
        streamAudioSource = new StreamAudioSource
            ("StreamAudioSource", context, null, null);

        String firstFile = reader.readLine();
        if (firstFile != null) {
            fileSetStream(getFirstToken(firstFile));
        }
    }


    /**
     * Returns the first token in the given line of text.
     *
     * @param text the text to tokenize
     *
     * @return the first token
     */
    public String getFirstToken(String text) {
        text = text.trim();
        int spaceIndex = text.indexOf(" ");
        if (spaceIndex > -1) {
            text = text.substring(0, spaceIndex);
        }
        return text;
    }


    /**
     * Construct an InputStream with the given audio file, and set it as
     * the InputStream of the streamAudioSource.
     *
     * @param audioFile the file containing audio data
     *
     * @throws java.io.IOException if error setting the stream
     */
    private void fileSetStream(String audioFile) throws IOException {
        streamAudioSource.setInputStream
            (new FileInputStream(audioFile), audioFile);
    }

    
    /**
     * Returns the next Audio object. 
     * Returns null if all the audio data in all the files have been read.
     *
     * @return the next Audio or <code>null</code> if no more is
     *     available
     *
     * @throws java.io.IOException
     */
    public Audio getAudio() throws IOException {
        if (streamAudioSource == null) {
            return null;
        }
        
        Audio frame = streamAudioSource.getAudio();
        if (frame != null) {
            return frame;
        } else {

            // assumes that the reader has already been opened
            if (reader != null) {
                // if we reached the end of the current file,
                // go to the next one
                String nextFile = reader.readLine();
                if (nextFile != null) {
                    fileSetStream(getFirstToken(nextFile));
                    return getAudio();
                } else {
                    reader.close();
                    reader = null;
                }
            }
        }

        return null;
    }
}


