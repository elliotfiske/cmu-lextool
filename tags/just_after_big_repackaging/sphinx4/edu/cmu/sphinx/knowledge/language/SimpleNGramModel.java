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

package edu.cmu.sphinx.knowledge.language;

import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.SphinxProperties;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.Reader;
import java.net.URL;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import java.util.Collections;
import java.util.StringTokenizer;

// for testing
import java.io.*;
import edu.cmu.sphinx.util.Timer;

/**
 * A simple ARPA language model loader. This loader makes no attempt
 * to optimize storage, so it can only load very small language models
 *
 * Note that all probabilites in the grammar are stored in LogMath log
 * base format. Language Probabilties in the language model file are
 * stored in log 10  base.
 */
public class SimpleNGramModel implements LanguageModel {

    private SphinxProperties props;
    private LogMath logMath;
    private Map map;
    private Set vocabulary;
    private int lineNumber;
    private String fileName;
    private BufferedReader reader;
    private int maxNGram = 0;
    
    /**
     * Creates a simple ngram model from the data at the URL. The
     * data should be an ARPA format
     *
     * @param context the context for this model
     *
     * @throws IOException if there is trouble loading the data
     */
    public SimpleNGramModel(String context) 
        throws IOException, FileNotFoundException {
            initialize(context);
    }
    
    /**
     * Raw constructor
     */
    public SimpleNGramModel() {
    }
    
    /**
     * Initializes this LanguageModel
     *
     * @param context the context to associate this linguist with
     */
    public void initialize(String context)  throws IOException {
        this.props = SphinxProperties.getSphinxProperties(context);
        
        String format = props.getString
            (LanguageModel.PROP_FORMAT, "sphinx4.v1");
        String location = props.getString
            (LanguageModel.PROP_LOCATION, ".");
        
        map = new HashMap();
        vocabulary = new HashSet();
        logMath = LogMath.getLogMath(context);
        load(format, location);
    }
    
    
    /**
     * Called before a recognitino
     */
    public void start() {
    }
    
    /**
     * Called after a recognition
     */
    public void stop() {
    }
    
    /**
     * Gets the ngram probability of the word sequence represented by
     * the word list 
     *
     * @param wordList a list of strings representing the word
     * sequence of interest.
     *
     * @return the probability of the word sequence.
     * Probability is in logMath log base
     *
     */
    public double getProbability(List wordList) {
        double logProbability = 0.0;

        Probability prob = getProb(wordList);
        if (prob == null) {
            if (wordList.size() > 1 ) {
                 logProbability = 
		     getBackoff(wordList.subList(0, wordList.size() -1)) 
		     + getProbability(wordList.subList(1, wordList.size()));
                       
            } else {    // if the single word is not in the model at all
                // then its zero likelihood that we'll use it   
                logProbability = logMath.getLogZero();
            }
        } else {
            logProbability = prob.logProbability;
        }
        if (false) {
            System.out.println(listToString(wordList) + " : " +
                    logProbability
                    + " " + logMath.logToLinear(logProbability));
        }
        return logProbability;
    }


    /**
     * Gets the ngram probability of the word sequence represented by
     * the word list 
     *
     * @param wordList a list of strings representing the word
     * sequence of interest.
     *
     * @return the probability of the word sequence.
     * Probability is in a linear base 
     *
     */
    public double getLinearProbability(List wordList) {
        double logProbability = getProbability(wordList);
        double linearProb = logMath.logToLinear(logProbability);
        return  linearProb;
    }
    
    /**
     * Returns the backoff probability for the give sequence of words
     *
     * @param the sequence of words
     *
     * @return the backoff probability in LogMath log base
     */
    public double getBackoff(List wordList) {
        double logBackoff = 0.0;           // log of 1.0
        Probability prob = getProb(wordList);
        if (prob != null) {
            logBackoff = prob.logBackoff;
        }
        return logBackoff;
    }
    
    /**
     * Returns the maximum depth of the language model
     *
     * @return the maximum depth of the language mdoel
     */
    public int getMaxDepth() {
        return maxNGram;
    }

     /**
      * Returns the set of words in the lanaguage model. The set is
      * unmodifiable.
      *
      * @return the unmodifiable set of words
      */
     public Set getVocabulary() {
         return Collections.unmodifiableSet(vocabulary);
     }
    
    /**
     * Gets the probability entry for the given word list or null if
     * there is no entry
     *
     * @param wordList a list of strings representing the word
     * sequence of interest.
     *
     * @return the probability entry for the wordlist or null
     */
    private Probability getProb(List wordList) {
        String rep = getRepresentation(wordList);
        return (Probability) map.get(rep);
    }
    
    /**
     * Gets the probability entry for the single word
     *
     * @param string the word of interest
     *
     * @return the probability entry or null
     */
    private Probability getProb(String string) {
        return (Probability) map.get(string);
    }
    

    /**
     * Converts a wordList to a string
     *
     * @param wordLIst the wordList
     *
     * @return the string
     */
    private String listToString(List wordList) {
        StringBuffer sb = new StringBuffer();
        for (Iterator i = wordList.iterator(); i.hasNext(); ) {
            sb.append(i.next().toString());
            sb.append(" ");
        }
        return sb.toString();
    }
    
    /**
     * Provides the log base that controls the range of probabilities
     * returned by this N-Gram
     */
    public void setLogMath(LogMath logMath) {
        this.logMath = logMath;
    }


    /**
     * Returns the log math the controls the log base for the range of
     * probabilities used by this n-gram
     */
    public LogMath getLogMath() {
        return this.logMath;
    }

    
    /**
     * Dumps the language model
     */
    public void dump() {
        for (Iterator i = map.keySet().iterator(); i.hasNext(); ) {
            String key = (String) i.next();
            Probability prob = (Probability) map.get(key);
            System.out.println(key + " " + prob);
        }
    }
    
    /**
     * Retrieves a string representation of the wordlist, suitable
     * for map access
     *
     * @param wordList the list of words
     *
     * @return a string representation of the word list
     */
    private String getRepresentation(List wordList) {
        StringBuffer sb = new StringBuffer();
        
        for (Iterator i = wordList.iterator(); i.hasNext(); ) {
            String s = (String) i.next();
            sb.append(s);
            if (i.hasNext()) {
                sb.append("+");
            }
        }
        return sb.toString();
    }
    
    
    /**
     * Loads the language model from the given location. 
     *
     * @param format the format of the model
     * @param location the location of the model
     * 
     * @throws IOException if an error occurs while loading
     */
    private void load(String format, String location) throws
    FileNotFoundException, IOException {
        
        String line;
        
        if (!format.equals("arpa")) {
            throw new IOException("Loading of " + format + 
                                  " language models not supported");
        }
        
        open(location);
        
        // look for beginning of data
        readUntil("\\data\\");
         
        // look for ngram statements
        
        List ngramList = new ArrayList();
        while ((line = readLine()) != null) {
            if (line.startsWith("ngram")) {
                StringTokenizer st = new StringTokenizer(line, " \t\n\r\f=");
                if (st.countTokens() != 3) {
                    corrupt("corrupt ngram field " + line + " " +
                            st.countTokens() );
                }
                st.nextToken();
                int index = Integer.parseInt(st.nextToken());
                int count = Integer.parseInt(st.nextToken());
                ngramList.add(index - 1, new Integer(count));
                if (index > maxNGram) {
                    maxNGram = index;
                }
            } else if (line.equals("\\1-grams:")) {
                break;
            }
        }
        
        for (int index = 0; index < ngramList.size(); index++) { 
            int ngram = index + 1;
            int ngramCount = ((Integer) ngramList.get(index)).intValue();
            for (int i = 0; i < ngramCount; i++) {
                StringTokenizer tok = new StringTokenizer(readLine());
                int tokenCount = tok.countTokens();
                if (tokenCount != ngram + 1 && tokenCount != ngram + 2) {
                    corrupt("Bad format");
                }
                double log10Prob = Double.parseDouble(tok.nextToken());
                double log10Backoff = 0.0;
                StringBuffer wordKey = new StringBuffer();
                for (int j = 0; j < ngram; j++) {
                    String word = tok.nextToken().toLowerCase();
                    vocabulary.add(word);
                    wordKey.append(word);
                    if (j < ngram - 1) {
                        wordKey.append("+");
                    }
                }
                if (tok.hasMoreTokens()) {
                    log10Backoff = Double.parseDouble(tok.nextToken());
                }
                put(wordKey.toString(), log10Prob, log10Backoff);
            }
            
            if (index < ngramList.size() - 1) {
                String next = "\\" + (ngram + 1) + "-grams:";
                readUntil(next);
            } 
        }
        readUntil("\\end\\");
        close();
    }
    
    
    
    /**
     * Puts the probability into the map
     * 
     * @param tag the tag for the prob.
     * @param log10Prob the probability in log base 10
     * @param log10Backoff the backoff probability in log base 10
     */
    private void put(String tag, double log10Prob, double log10Backoff) {
        map.put(tag.toLowerCase(), new Probability(
                  logMath.log10ToLog(log10Prob), 
                  logMath.log10ToLog(log10Backoff)));
    }
    
    /**
     * Reads the next line from the LM file. Keeps track of line
     * number.
     *
     * @throws IOException if an error occurs while reading the input
     * or an EOF is encountered.
     *
     */
    private String readLine() throws IOException {
        String line;
        lineNumber++;
        line = reader.readLine();
        if (line == null) {
            corrupt("Premature EOF");
        }
        return line;
    }
    
    /**
     * Opens the language model at the given location
     *
     * @param location the path to the language model
     *
     * @throws IOException if an error occurs while opening the file
     */
    private void open(String location) throws
    FileNotFoundException, IOException {
        lineNumber = 0;
        fileName = location;
        reader = new BufferedReader(new FileReader(location));
    }
    
    /**
     * Reads from the input stream until the input matches the given
     * string
     *
     * @param match the string to match on
     *
     * @throws IOException if an error occurs while reading the input
     * or an EOF is encountered before finding the match
     */
    private void readUntil(String match) throws IOException {
        try {
            while (!readLine().equals(match)) {
            }
        } catch (IOException ioe) {
            corrupt("Premature EOF while waiting for " + match);
        }
    }
    
    /**
     * Closes the language model file
     *
     * @throws IOException if an error occurs
     */
    private void close() throws IOException {
        reader.close();
        reader = null;
    }
    
    /**
     * Generates a 'corrupt' IO exception
     *
     * @throws an IOException with the given string
     */
    private void corrupt(String why) throws IOException {
        throw new IOException("Corrupt Language Model " + fileName +
                              " at line " +
                              lineNumber + ":" + why);
    }
    
    
    
    /**
     * A test routine
     *
     */
    public static void main(String[] args) throws Exception {
        SphinxProperties.initContext("test", new URL("file:./test.props"));
        SimpleNGramModel sm = new SimpleNGramModel("test");
        LogMath logMath = LogMath.getLogMath("test");
        
        BufferedReader reader = new BufferedReader
            (new InputStreamReader(System.in));
        
        String input;
        
        System.out.println("Max depth is " + sm.getMaxDepth());
        System.out.print("Enter words: ");
        while ((input = reader.readLine()) != null) {
            StringTokenizer st = new StringTokenizer(input);
            List list = new ArrayList();
            while (st.hasMoreTokens()) {
                String tok = (String) st.nextToken();
                list.add(tok.toLowerCase());
            }
            System.out.println("Probability of " + input + " is: " +
                           sm.getProbability(list) +
                           "(" +
                           logMath.logToLinear(sm.getProbability(list)) +
                           ")");
            System.out.print("Enter words: ");
        }
        
        
        Timer timer = Timer.getTimer("test", "lookup trigram");
        
        List list1 = new ArrayList();
        list1.add("t");
        list1.add("h");
        list1.add("e");
        
        List list2 = new ArrayList();
        list2.add("a");
        list2.add("l");
        list2.add("q");
        
        for (int i = 0; i < 1000000; i++) {
            timer.start();
            sm.getProbability(list1);
            timer.stop();
            timer.start();
            sm.getProbability(list2);
            timer.stop();
        }
        
        Timer.dumpAll("test");
    }
}

/**
 * Represents a probability and a backoff probability
 */
class Probability {
    double logProbability;
    double logBackoff;

    /**
     * Constructs a probability
     *
     * @param probability the probability
     * @param backoff the backoff probability
     */
    Probability(double logProbability, double logBackoff) {
        this.logProbability = logProbability;
        this.logBackoff = logBackoff;
    }
    
    /**
     * Returns a string representation of this object
     *
     * @return the string form of this object
     */
    public String toString() {
        return "Prob: " + logProbability + " " + logBackoff;
    }
};
