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

package edu.cmu.sphinx.knowledge.language.large;

import edu.cmu.sphinx.knowledge.dictionary.Dictionary;

import edu.cmu.sphinx.knowledge.language.LanguageModel;
import edu.cmu.sphinx.knowledge.language.WordSequence;

import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.Timer;
import edu.cmu.sphinx.util.Utilities;

import java.io.FileNotFoundException;
import java.io.IOException;

import java.net.URL;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;

import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Map;
import java.util.Set;
import java.util.HashSet;
import java.util.Collections;
import java.util.StringTokenizer;


/**
 * Queries a binary language model file generated by the 
 * CMU-Cambridge Statistical Language Modelling Toolkit.
 * 
 * Note that all probabilites in the grammar are stored in LogMath log
 * base format. Language Probabilties in the language model file are
 * stored in log 10  base. They are converted to the LogMath logbase.
 */
public class LargeTrigramModel implements LanguageModel {


    /**
     * The number of bytes per bigram in the LM file generated by the 
     * CMU-Cambridge Statistical Language Modelling Toolkit.
     */
    public static final int BYTES_PER_BIGRAM = 8;


    /**
     * The number of bytes per trigram in the LM file generated by the
     * CMU-Cambridge Statistical Language Modelling Toolkit.
     */
    public static final int BYTES_PER_TRIGRAM = 4;


    private SphinxProperties props;
    private LogMath logMath;

    private Map unigramIDMap;
    private Map loadedBigramBuffer;
    private Map loadedTrigramBuffer;
    private Map trigramCache;

    private UnigramProbability[] unigrams;

    private int startWordID;
    private int endWordID;

    private int[] trigramSegmentTable;

    private float[] bigramProbTable;
    private float[] trigramProbTable;
    private float[] trigramBackoffTable;

    private BinaryLoader loader;

    private int bigramMisses;
    private int trigramMisses;


    /**
     * Creates a simple ngram model from the data at the URL. The
     * data should be an ARPA format
     *
     * @param context the context for this model
     *
     * @throws IOException if there is trouble loading the data
     */
    public LargeTrigramModel(String context) 
        throws IOException, FileNotFoundException {
	initialize(context);
    }
    

    /**
     * Raw constructor
     */
    public LargeTrigramModel() {
    }

    
    /**
     * Initializes this LanguageModel
     *
     * @param context the context to associate this linguist with
     */
    public void initialize(String context) throws IOException {

        Timer.start("LM Load");

        this.props = SphinxProperties.getSphinxProperties(context);
        
        String format = props.getString
            (LanguageModel.PROP_FORMAT, LanguageModel.PROP_FORMAT_DEFAULT);
        String location = props.getString
            (LanguageModel.PROP_LOCATION, LanguageModel.PROP_LOCATION_DEFAULT);

        unigramIDMap = new HashMap();
        loadedBigramBuffer = new HashMap();
	loadedTrigramBuffer = new HashMap();
	trigramCache = new HashMap();
        logMath = LogMath.getLogMath(context);

        loader = new BinaryLoader(context);

        unigrams = loader.getUnigrams();
        bigramProbTable = loader.getBigramProbabilities();
        trigramProbTable = loader.getTrigramProbabilities();
        trigramBackoffTable = loader.getTrigramBackoffWeights();
        trigramSegmentTable = loader.getTrigramSegments();
        buildUnigramIDMap();

        Timer.stop("LM Load");
    }


    /**
     * Builds the map from unigram to unigramID.
     * Also finds the startWordID and endWordID.
     */
    private void buildUnigramIDMap() {
        String[] words = loader.getWords();
        for (int i = 0; i < words.length; i++) {
            if (words[i].equals(Dictionary.SENTENCE_START_SPELLING)) {
                this.startWordID = i;
            } else if (words[i].equals(Dictionary.SENTENCE_END_SPELLING)) {
                this.endWordID = i;
            }
            unigramIDMap.put(words[i], unigrams[i]);
        }
    }
    
    
    /**
     * Called before a recognition
     */
    public void start() {
        
        loadedBigramBuffer.clear();
        loadedTrigramBuffer.clear();
        trigramCache.clear();
        
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
     * @param wordSequence the word sequence
     *
     * @return the probability of the word sequence.
     * Probability is in logMath log base
     *
     */
    public float getProbability(WordSequence wordSequence) {
        if (wordSequence.size() == 3) {
            return getTrigramProbability(wordSequence);
        } else if (wordSequence.size() == 2) {
            return getBigramProbability(wordSequence);
	} else if (wordSequence.size() == 1) {
	    return getUnigramProbability(wordSequence);
	} else {
            throw new Error("Unsupported N-gram: " + wordSequence.size());
        }
    }


    /**
     * Returns the unigram probability of the given unigram.
     *
     * @param wordSequence the unigram word sequence
     *
     * @return the unigram probability
     */
    private float getUnigramProbability(WordSequence wordSequence) {
        String unigram = wordSequence.getWord(0);
        UnigramProbability unigramProb = getUnigram(unigram);
        if (unigramProb == null) {
            throw new Error("Unigram not in LM: " + unigram);
        }
        return unigramProb.getLogProbability();
    }


    /**
     * Returns true if this language model has the given unigram.
     *
     * @param unigram the unigram to find
     *
     * @return true if this LM has this unigram, false otherwise
     */
    private UnigramProbability getUnigram(String unigram) {
        return (UnigramProbability) unigramIDMap.get(unigram.toLowerCase());
    }


    /**
     * Returns the ID of the given word.
     *
     * @param word the word to find the ID
     *
     * @return the ID of the word
     */
    private final int getWordID(String word) {
	UnigramProbability probability = getUnigram(word);
        if (probability == null) {
            throw new IllegalArgumentException("No word ID: " + word);
        } else {
            return probability.getWordID();
        }
    }


    /**
     * Returns the unigram probability of the given unigram.
     *
     * @param wordSequence the unigram word sequence
     *
     * @return the unigram probability
     */
    private float getBigramProbability(WordSequence wordSequence) {

        String firstWord = wordSequence.getWord(0);
        String secondWord = wordSequence.getWord(1);

        if (loader.getNumberBigrams() <= 0 || firstWord == null) {
            return getUnigramProbability(wordSequence.getNewest());
        }
        if (getUnigram(secondWord) == null) {
            throw new Error("Bad word2: " + secondWord);
        }

        int firstWordID = getWordID(firstWord);
        int secondWordID = getWordID(secondWord);
        
        BigramProbability bigram = findBigram(firstWordID, secondWordID);

        if (bigram != null) {
            return bigramProbTable[bigram.getProbabilityID()];
        } else {
            // System.out.println("Didn't find bigram");
            bigramMisses++;
            return (unigrams[firstWordID].getLogBackoff() + 
                    unigrams[secondWordID].getLogProbability());
        }
    }


    /**
     * Finds the BigramProbability for a particular bigram
     *
     * @param firstWordID the first word in the bigram
     * @param secondWordID the second word in the bigram
     *
     * @return the BigramProbability of the bigram, or null if the
     *         given first word has no bigrams
     */
    private BigramProbability findBigram(int firstWordID, int secondWordID) {
        BigramProbability bigram = null;
        BigramBuffer bigrams = getBigramBuffer(firstWordID);
        if (bigrams != null) {
            bigram = bigrams.findBigram(secondWordID);
        }
        return bigram;
    }


    /**
     * Returns the bigrams of the given word
     *
     * @param firstWordID the ID of the word
     *
     * @return the bigrams of the word
     */
    private BigramBuffer getBigramBuffer(int firstWordID) {
        Integer key = new Integer(firstWordID);
        BigramBuffer bigramBuffer = 
            (BigramBuffer) loadedBigramBuffer.get(key);
        if (bigramBuffer == null) {
            int numberBigrams = getNumberBigramFollowers(firstWordID);
            bigramBuffer = loadBigramBuffer(firstWordID, numberBigrams);
            if (bigramBuffer != null) {
                loadedBigramBuffer.put(key, bigramBuffer);
            }
        }
        return bigramBuffer;
    }


    /**
     * Returns the number of bigram followers of a word.
     *
     * @param wordID the ID of the word
     *
     * @return the number of bigram followers
     */
    private int getNumberBigramFollowers(int wordID) {
        return unigrams[wordID+1].getFirstBigramEntry() -
            unigrams[wordID].getFirstBigramEntry();
    }


    /**
     * Returns the language probability of the given trigram.
     *
     * @param wordSequence the trigram word sequence
     *
     * @return the trigram probability
     */
    private float getTrigramProbability(WordSequence wordSequence) {
        if (loader.getNumberTrigrams() == 0) {
            return getBigramProbability(wordSequence.getNewest());
        }

        TrigramProbability trigram = 
	    (TrigramProbability) trigramCache.get(wordSequence);
        if (trigram != null) {
            // System.out.println("Found trigram!");
            return trigramProbTable[trigram.getProbabilityID()];
        }
	
       	float score = 0.0f;
	trigram = findTrigram(wordSequence);

	if (trigram != null) {
	    trigramCache.put(wordSequence, trigram);
            // System.out.println("Found trigram!");
	    score = trigramProbTable[trigram.getProbabilityID()];
	} else {
            trigramMisses++;
            
            int firstWordID = getWordID(wordSequence.getWord(0));
            int secondWordID = getWordID(wordSequence.getWord(1));
            
	    BigramProbability bigram = findBigram(firstWordID, secondWordID);
	    if (bigram != null) {
		score = trigramBackoffTable[bigram.getBackoffID()] +
		    getBigramProbability(wordSequence.getNewest());
	    } else {
		score = getBigramProbability(wordSequence.getNewest());
	    }
	}

	return score;
    }


    /**
     * Finds or loads the trigram probability of the given trigram.
     *
     * @param wordSequence the trigram to load
     *
     * @return a TrigramProbability of the given trigram
     */
    private TrigramProbability findTrigram(WordSequence wordSequence) {
	WordSequence oldest = wordSequence.getOldest();
	TrigramBuffer trigramBuffer = 
	    (TrigramBuffer)loadedTrigramBuffer.get(oldest);

	int thirdWordID = getWordID(wordSequence.getWord(2));

	if (trigramBuffer == null) {
	    int firstWordID = getWordID(wordSequence.getWord(0));
	    int secondWordID = getWordID(wordSequence.getWord(1));
	    trigramBuffer = loadTrigramBuffer(firstWordID, secondWordID);
            if (trigramBuffer != null) {
                loadedTrigramBuffer.put(oldest, trigramBuffer);
            } else {
                return null;
            }
	}
	return trigramBuffer.findTrigram(thirdWordID);
    }


    /**
     * Loads into a buffer all the trigram followers of the given bigram.
     *
     * @param firstWordID the ID of the first word
     * @param secondWordID the ID of the second word
     *
     * @return a TrigramBuffer of all the trigram followers of the
     *    given two words
     */
    private TrigramBuffer loadTrigramBuffer(int firstWordID, 
                                            int secondWordID) {
        TrigramBuffer trigramBuffer = null;

        BigramBuffer bigramBuffer = getBigramBuffer(firstWordID);

        if (bigramBuffer != null) {
            BigramProbability bigram = bigramBuffer.findBigram(secondWordID);
            
            if (bigram != null) {

                BigramProbability nextBigram = 
                    bigramBuffer.getBigramProbability
                    (bigram.getWhichFollower() + 1);

                int firstBigramEntry = 
                    unigrams[firstWordID].getFirstBigramEntry();
                int firstTrigramEntry = 
                    getFirstTrigramEntry(bigram, firstBigramEntry);
                int numberTrigrams = 
                    getFirstTrigramEntry(nextBigram, firstBigramEntry) - 
                    firstTrigramEntry;
                int size = numberTrigrams * BYTES_PER_TRIGRAM;
                long position = 
                    (loader.getTrigramOffset() + 
                     (long) (firstTrigramEntry * BYTES_PER_TRIGRAM));

                try {
                    // System.out.println("Loading TrigramBuffer from disk");
                    byte[] buffer = loader.loadBuffer(position, size);
                    trigramBuffer = new TrigramBuffer(buffer, numberTrigrams,
						      loader.getBigEndian());
                } catch (IOException ioe) {
                    ioe.printStackTrace();
                    throw new Error("Error loading trigrams.");
                }
            }
        }
        return trigramBuffer;
    }

    
    /**
     * Returns the index of the first trigram entry of the given bigram
     *
     * @param bigram the bigram which first trigram entry we're looking for
     * @param firstBigramEntry the index of the first bigram entry 
     *        of the bigram in question
     *
     * @return the index of the first trigram entry of the given bigram
     */
    private int getFirstTrigramEntry(BigramProbability bigram,
                                           int firstBigramEntry) {
        int firstTrigramEntry = 
            trigramSegmentTable[(firstBigramEntry+bigram.getWhichFollower()) >>
                               loader.getLogBigramSegmentSize()] + 
            bigram.getFirstTrigramEntry();
        return firstTrigramEntry;
    }


    /**
     * Returns the backoff probability for the give sequence of words
     *
     * @param wordSequence the sequence of words
     *
     * @return the backoff probability in LogMath log base
     */
    public float getBackoff(WordSequence wordSequence) {
        float logBackoff = 0.0f;           // log of 1.0
        UnigramProbability prob = null; //getProb(wordSequence);
        if (prob != null) {
            logBackoff = prob.getLogBackoff();
        }
        return logBackoff;
    }
    

    /**
     * Returns the maximum depth of the language model
     *
     * @return the maximum depth of the language mdoel
     */
    public int getMaxDepth() {
        return loader.getMaxDepth();
    }


    /**
     * Returns the set of words in the lanaguage model. The set is
     * unmodifiable.
     *
     * @return the unmodifiable set of words
     */
    public Set getVocabulary() {
        Set vocabulary = new HashSet();
        vocabulary.addAll(Arrays.asList(loader.getWords()));
        return Collections.unmodifiableSet(vocabulary);
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
     * Loads the bigram followers of the given first word in a bigram from
     * disk to memory. It actually loads (numberFollowers + 1) bigrams,
     * since we need the first bigram of the next word to determine
     * the number of trigrams of the last bigram.
     *
     * @param firstWordID ID of the first word
     * @param numberFollowers the number of bigram followers this word has
     *
     * @return the bigram followers of the given word
     */
    private BigramBuffer loadBigramBuffer(int firstWordID, 
					  int numberFollowers) {
        BigramBuffer followers = null;
	int firstBigramEntry = unigrams[firstWordID].getFirstBigramEntry();
	int size = (numberFollowers + 1) * BYTES_PER_BIGRAM;
	long position = (long) (loader.getBigramOffset() + 
				(firstBigramEntry * BYTES_PER_BIGRAM));
	try {
	    byte[] buffer = loader.loadBuffer(position, size);
	    followers = new BigramBuffer(buffer, numberFollowers + 1,
					 loader.getBigEndian());
	} catch (IOException ioe) {
	    ioe.printStackTrace();
	    throw new Error("Error loading bigram followers");
	}
	
	return followers;
    }

    
    /**
     * Returns the number of times when a bigram is queried, but there
     * is no bigram in the LM (in which case it uses the backoff 
     * probabilities).
     *
     * @return the number of bigram misses
     */
    public int getBigramMisses() {
        return bigramMisses;
    }


    /**
     * Returns the number of times when a trigram is queried, but there
     * is no trigram in the LM (in which case it uses the backoff 
     * probabilities).
     *
     * @return the number of bigram misses
     */
    public int getTrigramMisses() {
        return trigramMisses;
    }
}

