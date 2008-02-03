/*
 * 
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
package edu.cmu.sphinx.util.props;

import org.xml.sax.InputSource;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParserFactory;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

/** Loads configuration from an XML file */
public class SaxLoader {

    private URL url;
    private Map<String, RawPropertyData> rpdMap;
    private GlobalProperties globalProperties;


    /**
     * Creates a loader that will load from the given location
     *
     * @param url              the location to load
     * @param globalProperties the map of global properties
     */
    public SaxLoader(URL url, GlobalProperties globalProperties) {
        this.url = url;
        this.globalProperties = globalProperties;
    }


    /**
     * Loads a set of configuration data from the location
     *
     * @return a map keyed by component name containing RawPropertyData objects
     * @throws IOException if an I/O or parse error occurs
     */
    public Map<String, RawPropertyData> load() throws IOException {
        rpdMap = new HashMap<String, RawPropertyData>();
        try {
            SAXParserFactory factory = SAXParserFactory.newInstance();
            XMLReader xr = factory.newSAXParser().getXMLReader();
            ConfigHandler handler = new ConfigHandler(rpdMap, globalProperties);
            xr.setContentHandler(handler);
            xr.setErrorHandler(handler);
            InputStream is = url.openStream();
            xr.parse(new InputSource(is));
            is.close();
        } catch (SAXParseException e) {
            String msg = "Error while parsing line " + e.getLineNumber()
                    + " of " + url + ": " + e.getMessage();
            throw new IOException(msg);
        } catch (SAXException e) {
            throw new IOException("Problem with XML: " + e);
        } catch (ParserConfigurationException e) {
            throw new IOException(e.getMessage());
        }
        return rpdMap;
    }
}
