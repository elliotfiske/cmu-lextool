/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.linguist.language.ngram;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Dictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.props.ConfigurationManagerUtils;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.S4Component;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.URL;
import java.util.*;
import java.util.logging.Logger;

/**
 * An ASCII ARPA language model loader. This loader makes no attempt to optimize storage, so it can only load very small
 * language models
 * <p/>
 * Note that all probabilities in the grammar are stored in LogMath log base format. Language Probabilities in the
 * language model file are stored in log 10 base.
 */
public class SimpleNGramModel implements LanguageModel {

    /** Sphinx property that defines the logMath component. */
    @S4Component(type = LogMath.class)
    public final static String PROP_LOG_MATH = "logMath";
    // ----------------------------
    // Configuration data
    // ----------------------------
    private String name;
    private LogMath logMath;
    private String format;
    private URL urlLocation;
    private float unigramWeight;
    private Dictionary dictionary;
    private int desiredMaxDepth;
    private Logger logger;
    private int maxNGram = 0;
    private Map<WordSequence, Probability> map;
    private Set<String> vocabulary;
    protected int lineNumber;
    protected BufferedReader reader;
    protected String fileName;
    private boolean allocated = false;


    public Logger getLogger() {
        return logger;
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
    */
    public void newProperties(PropertySheet ps) throws PropertyException {


        if (allocated) {
            throw new RuntimeException("Can't change properties after allocation");
        }

        logger = ps.getLogger();
        format = ps.getString(PROP_FORMAT);
        urlLocation = ConfigurationManagerUtils.getResource(PROP_LOCATION, ps);
        unigramWeight = ps.getFloat(PROP_UNIGRAM_WEIGHT
        );
        logMath = (LogMath) ps.getComponent(PROP_LOG_MATH);
        desiredMaxDepth = ps.getInt(PROP_MAX_DEPTH);
        dictionary = (Dictionary) ps.getComponent(PROP_DICTIONARY
        );
        map = new HashMap<WordSequence, Probability>();
        vocabulary = new HashSet<String>();
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.linguist.language.ngram.LanguageModel#allocate()
    */
    public void allocate() throws IOException {
        allocated = true;
        load(format, urlLocation, unigramWeight, dictionary);
        if (desiredMaxDepth > 0) {
            if (desiredMaxDepth < maxNGram) {
                maxNGram = desiredMaxDepth;
            }
        }
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.linguist.language.ngram.LanguageModel#deallocate()
    */
    public void deallocate() {
        allocated = false;
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.util.props.Configurable#getName()
    */
    public String getName() {
        return name;
    }


    /** Called before a recognition */
    public void start() {
    }


    /** Called after a recognition */
    public void stop() {
    }


    /**
     * Gets the ngram probability of the word sequence represented by the word list
     *
     * @param wordSequence the word sequence
     * @return the probability of the word sequence. Probability is in logMath log base
     */
    public float getProbability(WordSequence wordSequence) {
        float logProbability = 0.0f;
        Probability prob = getProb(wordSequence);
        if (prob == null) {
            if (wordSequence.size() > 1) {
                logProbability = getBackoff(wordSequence.getOldest())
                        + getProbability(wordSequence.getNewest());
            } else { // if the single word is not in the model at all
                // then its zero likelihood that we'll use it
                logProbability = LogMath.getLogZero();
            }
        } else {
            logProbability = prob.logProbability;
        }
         //    System.out.println("Search: " + wordSequence + " : "
         //           + logProbability + " "
         //           + logMath.logToLinear(logProbability));
         return logProbability;
    }


    /**
     * Gets the smear term for the given wordSequence
     *
     * @param wordSequence the word sequence
     * @return the smear term associated with this word sequence
     */
    public float getSmear(WordSequence wordSequence) {
        return 0.0f; // TODO not implemented
    }


    /**
     * Returns the backoff probability for the give sequence of words
     *
     * @param wordSequence the sequence of words
     * @return the backoff probability in LogMath log base
     */
    public float getBackoff(WordSequence wordSequence) {
        float logBackoff = 0.0f; // log of 1.0
        Probability prob = getProb(wordSequence);
        if (prob != null) {
            logBackoff = prob.logBackoff;
        }
        return logBackoff;
    }


    /**
     * Returns the maximum depth of the language model
     *
     * @return the maximum depth of the language model
     */
    public int getMaxDepth() {
        return maxNGram;
    }


    /**
     * Returns the set of words in the language model. The set is unmodifiable.
     *
     * @return the unmodifiable set of words
     */
    public Set<String> getVocabulary() {
        return Collections.unmodifiableSet(vocabulary);
    }


    /**
     * Gets the probability entry for the given word sequence or null if there is no entry
     *
     * @param wordSequence a word sequence
     * @return the probability entry for the wordlist or null
     */
    private Probability getProb(WordSequence wordSequence) {
        return map.get(wordSequence);
    }


    /**
     * Converts a wordList to a string
     *
     * @param wordList the wordList
     * @return the string
     */
    @SuppressWarnings("unused")
	private String listToString(List<Word> wordList) {
        StringBuffer sb = new StringBuffer();
        for (Word word : wordList) {
            sb.append(word.toString());
            sb.append(" ");
        }
        return sb.toString();
    }


    /** Dumps the language model */
    public void dump() {
        for (Iterator<WordSequence> i = map.keySet().iterator(); i.hasNext();) {
            WordSequence ws = i.next();
            Probability prob = map.get(ws);
            System.out.println(ws.toString() + " " + prob);
        }
    }


    /**
     * Retrieves a string representation of the wordlist, suitable for map access
     *
     * @param wordList the list of words
     * @return a string representation of the word list
     */
    @SuppressWarnings("unused")
	private String getRepresentation(List<String> wordList) {
        StringBuffer sb = new StringBuffer();
        for (Iterator<String> i = wordList.iterator(); i.hasNext();) {
            String s = i.next();
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
     * @param format        the format of the model
     * @param location      the URL location of the model
     * @param unigramWeight the unigram weight
     * @throws IOException if an error occurs while loading
     */
    private void load(String format, URL location, float unigramWeight,
                      Dictionary dictionary) throws FileNotFoundException, IOException {
        String line;
        float logUnigramWeight = logMath.linearToLog(unigramWeight);
        float inverseLogUnigramWeight = logMath
                .linearToLog(1.0 - unigramWeight);
        if (!format.equals("arpa")) {
            throw new IOException("Loading of " + format
                    + " language models not supported");
        }
        open(location);
        // look for beginning of data
        readUntil("\\data\\");
        // look for ngram statements
        List<Integer> ngramList = new ArrayList<Integer>();
        while ((line = readLine()) != null) {
            if (line.startsWith("ngram")) {
                StringTokenizer st = new StringTokenizer(line, " \t\n\r\f=");
                if (st.countTokens() != 3) {
                    corrupt("corrupt ngram field " + line + " "
                            + st.countTokens());
                }
                st.nextToken();
                int index = Integer.parseInt(st.nextToken());
                int count = Integer.parseInt(st.nextToken());
                ngramList.add(index - 1, count);
                if (index > maxNGram) {
                    maxNGram = index;
                }
            } else if (line.equals("\\1-grams:")) {
                break;
            }
        }
        int numUnigrams = (ngramList.get(0)).intValue() - 1;
        // -log(x) = log(1/x)
        float logUniformProbability = -logMath.linearToLog(numUnigrams);
        for (int index = 0; index < ngramList.size(); index++) {
            int ngram = index + 1;
            int ngramCount = (ngramList.get(index)).intValue();
            for (int i = 0; i < ngramCount; i++) {
                StringTokenizer tok = new StringTokenizer(readLine());
                int tokenCount = tok.countTokens();
                if (tokenCount != ngram + 1 && tokenCount != ngram + 2) {
                    corrupt("Bad format");
                }
                float log10Prob = Float.parseFloat(tok.nextToken());
                float log10Backoff = 0.0f;
                // construct the WordSequence for this N-Gram
                List<Word> wordList = new ArrayList<Word>(maxNGram);
                for (int j = 0; j < ngram; j++) {
                    String word = tok.nextToken().toLowerCase();
                    vocabulary.add(word);
                    Word wordObject = dictionary.getWord(word);
                    if (wordObject == null) {
                        wordObject = Word.UNKNOWN;
                    }
                    wordList.add(wordObject);
                }
                WordSequence wordSequence = new WordSequence(wordList);
                if (tok.hasMoreTokens()) {
                    log10Backoff = Float.parseFloat(tok.nextToken());
                }
                float logProb = logMath.log10ToLog(log10Prob);
                float logBackoff = logMath.log10ToLog(log10Backoff);
                // Apply unigram weights if this is a unigram probability
                if (ngram == 1) {
                    float p1 = logProb + logUnigramWeight;
                    float p2 = logUniformProbability + inverseLogUnigramWeight;
                    logProb = logMath.addAsLinear(p1, p2);
//                    System.out
//                    .println("p1 " + p1 + " p2 " + p2 + " luw "
//                    		+ logUnigramWeight + " iluw "
//                    		+ inverseLogUnigramWeight + " lup "
//                    		+ logUniformProbability + " logprog "
//                    		+ logProb);
                }
                put(wordSequence, logProb, logBackoff);
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
     * @param wordSequence the tag for the prob.
     * @param logProb      the probability in log math base
     * @param logBackoff   the backoff probability in log math base
     */
    private void put(WordSequence wordSequence, float logProb, float logBackoff) {
//        System.out.println("Putting " + wordSequence + " p " + logProb
//                            + " b " + logBackoff);
        map.put(wordSequence, new Probability(logProb, logBackoff));
    }


    /**
     * Reads the next line from the LM file. Keeps track of line number.
     *
     * @throws IOException if an error occurs while reading the input or an EOF is encountered.
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
     * @throws IOException if an error occurs while opening the file
     */
    private void open(URL location) throws FileNotFoundException,
            IOException {
        lineNumber = 0;
        fileName = location.toString();
        reader = new BufferedReader
                (new InputStreamReader(location.openStream()));
    }


    /**
     * Reads from the input stream until the input matches the given string
     *
     * @param match the string to match on
     * @throws IOException if an error occurs while reading the input or an EOF is encountered before finding the match
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
     * @throws IOException with the given string
     */
    private void corrupt(String why) throws IOException {
        throw new IOException("Corrupt Language Model " + fileName
                + " at line " + lineNumber + ":" + why);
    }
}

/** Represents a probability and a backoff probability */

class Probability {

    float logProbability;
    float logBackoff;


    /**
     * Constructs a probability
     *
     * @param logProbability the probability
     * @param logBackoff     the backoff probability
     */
    Probability(float logProbability, float logBackoff) {
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
}
