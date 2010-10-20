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

package edu.cmu.sphinx.decoder;

import edu.cmu.sphinx.frontend.util.StreamDataSource;
import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.Timer;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;
import java.net.URL;
import java.util.StringTokenizer;


/**
 * A live decoder.
 */
public class BatchAligner {


    /**
     * Sphinx property to indicate the number of files to skip after every
     * decode.
     */
    public final static String PROP_SKIP = 
        "edu.cmu.sphinx.decoder.BatchAligner.skip";


    /**
     * Default value for the property PROP_SKIP.
     */
    public final static int PROP_SKIP_DEFAULT = 0;


    private StreamDataSource audioSource;
    private Decoder decoder;
    private String batchFile;
    private String context;
    private int skip;


    /**
     * Constructs a BatchAligner.
     *
     * @param context the context of this BatchAligner
     * @param batchFile the file that contains a list of files to decode
     *
     * @throws InstantiationException if the aligner could not be  created
     * @throws IOException if the aligner could not be loaded
     */
    public BatchAligner(String context, String batchFile) 
            throws IOException, InstantiationException  {
	this.context = context;
	SphinxProperties props = SphinxProperties.getSphinxProperties(context);
	skip = props.getInt(PROP_SKIP, PROP_SKIP_DEFAULT);
        audioSource = new StreamDataSource();
        audioSource.initialize("batchAudioSource", null, props, null);
        decoder = new Decoder(context);
        decoder.initialize();
        decoder.getRecognizer().getFrontEnd().setDataSource(audioSource);
        this.batchFile = batchFile;
    }


    /**
     * Decodes the batch of audio files
     */
    public void decode() throws IOException {

	int curCount = skip;
        BufferedReader reader = new BufferedReader(new FileReader(batchFile));

        String line = null;

        System.out.println("\nBatchAligner: decoding files in " + batchFile);
        System.out.println("----------");

        while ((line = reader.readLine()) != null) {
	    StringTokenizer st = new StringTokenizer(line);
	    String ref = null;
	    String file = (String) st.nextToken();
	    StringBuffer reference = new StringBuffer();

	    while (st.hasMoreTokens()) {
		reference.append((String) st.nextToken());
		reference.append(" ");
	    }

	    if (reference.length() > 0) {
		ref = reference.toString();
	    }
	    if (++curCount >= skip) {
		curCount = 0;
		decodeFile(file, ref);
	    }
        }

        System.out.println("\nBatchAligner: All files decoded\n");
        Timer.dumpAll(context);
	decoder.showSummary();
    }


    /**
     * Decodes the given file.
     *
     * @param file the file to decode
     * @param ref the reference string (or null if not available)
     */
    public void decodeFile(String file, String ref) throws IOException {

        System.out.println("\nDecoding: " + file);

        audioSource.setInputStream(new FileInputStream(file), null);

        // usually 25 features in one audio frame
        // but it doesn't really matter what this number is
        decoder.align(ref);
    }



    /**
     * Returns only the file name of the given full file path.
     * For example, "/usr/java/bin/javac" will return "javac".
     *
     * @return the file name of the given full file path
     */
    private static String getFilename(String fullPath) {
        int lastSlash = fullPath.lastIndexOf(File.separatorChar);
        return fullPath.substring(lastSlash+1);
    }


    /**
     * Main method of this BatchAligner.
     *
     * @param argv argv[0] : SphinxProperties file
     *             argv[1] : a file listing all the audio files to decode
     */
    public static void main(String[] argv) {

        if (argv.length < 2) {
            System.out.println
                ("Usage: BatchAligner propertiesFile batchFile");
            System.exit(1);
        }

        String context = "batch";
        String propertiesFile = argv[0];
        String batchFile = argv[1];
        String pwd = System.getProperty("user.dir");

        try {
            SphinxProperties.initContext
                (context, new URL("file://" + pwd + 
                                  File.separatorChar + propertiesFile));

            BatchAligner decoder = new BatchAligner(context, batchFile);
            decoder.decode();

        } catch (IOException ioe) {
            ioe.printStackTrace();
        } catch (InstantiationException ie) {
            ie.printStackTrace();
        }
    }
}