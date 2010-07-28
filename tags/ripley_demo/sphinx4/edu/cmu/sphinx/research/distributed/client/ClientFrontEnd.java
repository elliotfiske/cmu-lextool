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


package edu.cmu.sphinx.research.distributed.client;

import edu.cmu.sphinx.frontend.AudioSource;
import edu.cmu.sphinx.frontend.Cepstrum;
import edu.cmu.sphinx.frontend.CepstrumExtractor;
import edu.cmu.sphinx.frontend.Signal;

import edu.cmu.sphinx.frontend.util.StreamAudioSource;

import edu.cmu.sphinx.util.SphinxProperties;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;

import java.net.Socket;



/**
 * A FrontEnd that runs on client applications.
 * The main interface between the application and this ClientFrontEnd is
 * the <b>decode(InputStream inputStream, String streamName)</b> method,
 * which returns the decoded result as a String.
 *
 * <p>The method <b>connect()</b> should be called before <b>decode()</b>,
 * and the method <b>close()</b> should be called after all decoding is done.
 * Therefore, the correct sequence of calls is:
 * <code>
 * connect();
 * decode(inputstream1, name1);
 * ...
 * decode(inputstreamN, nameN);
 * close();
 * </code>
 */
public interface ClientFrontEnd {


    /**
     * Prefix string for the properties.
     */
    public static final String PROP_PREFIX
         = "edu.cmu.sphinx.research.distributed.client.ClientFrontEnd.";


    /**
     * The SphinxProperty that specifies the decoder server address.
     */
    public static final String PROP_SERVER = PROP_PREFIX + "server";


    /**
     * The default value of PROP_SERVER.
     */
    public static final String PROP_SERVER_DEFAULT = "localhost";


    /**
     * The SphinxProperty that specified the server port number.
     */
    public static final String PROP_PORT = PROP_PREFIX + "port";


    /**
     * The default value of PROP_PORT.
     */
    public static final int PROP_PORT_DEFAULT = 52703;
    

    /**
     * Constructs a default ClientFrontEnd.
     *
     * @param name the name of this ClientFrontEnd
     * @param context the context of this ClientFrontEnd
     */
    public void initialize(String name, String context) throws IOException;


    /**
     * Connects this ClientFrontEnd to the back-end server.
     */
    public void connect();


    /**
     * Closes the connection to the back-end server.
     */
    public void close();

    
    /**
     * Decodes the data in the given InputStream.
     *
     * @param is the InputStream to decode
     * @param streamName the name of the InputStream
     *
     * @return the result string
     */
    public String decode(InputStream is, String streamName) throws IOException;

}
