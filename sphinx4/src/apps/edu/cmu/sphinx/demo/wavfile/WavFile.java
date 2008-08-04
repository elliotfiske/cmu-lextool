/*
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

package edu.cmu.sphinx.demo.wavfile;

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import edu.cmu.sphinx.util.props.PropertyException;
import org.junit.Assert;
import org.junit.Test;

import javax.sound.sampled.AudioSystem;
import javax.sound.sampled.UnsupportedAudioFileException;
import java.io.File;
import java.io.IOException;
import java.net.URL;


/**
 * A simple Sphinx-4 application that decodes a .WAV file containing connnected-digits audio data. The audio format
 * itself should be PCM-linear, with the sample rate, bits per sample, sign and endianness as specified in the
 * config.xml file.
 */
public class WavFile {

    public static Result result;


    /** Main method for running the WavFile demo. */
    public static void main(String[] args) {
        try {

            URL audioFileURL;

            if (args.length > 0) {
                audioFileURL = new File(args[0]).toURI().toURL();
            } else {
                audioFileURL = WavFile.class.getResource("12345.wav");
            }

            URL configURL = WavFile.class.getResource("config.xml");

            System.out.println("Loading Recognizer as defined in '" + configURL.toString() + "'...\n");

            ConfigurationManager cm = new ConfigurationManager(configURL);

            Recognizer recognizer = (Recognizer) cm.lookup("recognizer");

            /* allocate the resource necessary for the recognizer */
            recognizer.allocate();

            System.out.println("Decoding " + audioFileURL.getFile());
            System.out.println(AudioSystem.getAudioFileFormat(audioFileURL));

            AudioFileDataSource dataSource = (AudioFileDataSource) cm.lookup("audioFileDataSource");
            dataSource.setAudioFile(audioFileURL, null);
            
            /* decode the audio file */
            result = recognizer.recognize();

            /* print out the results */
            if (result != null) {
                System.out.println("\nRESULT: " +
                        result.getBestFinalResultNoFiller() + "\n");
            } else {
                System.out.println("Result: null\n");
            }
        } catch (IOException e) {
            System.err.println("Problem when loading WavFile: " + e);
            throw new RuntimeException(e);
        } catch (PropertyException e) {
            System.err.println("Problem configuring WavFile: " + e);
            throw new RuntimeException(e);
        } catch (UnsupportedAudioFileException e) {
            System.err.println("Audio file format not supported: " + e);
            throw new RuntimeException(e);
        }
    }


    /** Converts this demo into a unit-test. */
    @Test
    public void testWavFileDemo() {
        try {
            WavFile.main(new String[]{});
        } catch (Throwable t) {
            t.printStackTrace();
            Assert.fail();
        }

        Assert.assertNotNull(result);
        Assert.assertTrue(result.getBestResultNoFiller().equals("one two three four five"));
    }
}
