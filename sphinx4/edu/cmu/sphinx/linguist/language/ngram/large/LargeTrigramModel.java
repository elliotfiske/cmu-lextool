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

package edu.cmu.sphinx.linguist.language.ngram.large;

import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Set;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Dictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.LanguageModel;
import edu.cmu.sphinx.linguist.language.ngram.large.BigramBuffer;
import edu.cmu.sphinx.linguist.language.ngram.large.BigramProbability;
import edu.cmu.sphinx.linguist.language.ngram.large.TrigramBuffer;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.Timer;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.PropertyType;
import edu.cmu.sphinx.util.props.Registry;

/**
 * Queries a binary language model file generated by the 
 * <a href="http://www.speech.cs.cmu.edu/SLM_info.html">
 * CMU-Cambridge Statistical Language Modelling Toolkit</a>.
 * 
 * Note that all probabilites in the grammar are stored in LogMath log base
 * format. Language Probabilties in the language model file are stored in log
 * 10 base. They are converted to the LogMath logbase.
 */
public class LargeTrigramModel implements LanguageModel {
    /**
     * Sphinx property for the name of the file that logs all the queried
     * N-grams. If this property is set to null, it means that the queried
     * N-grams are not logged.
     */
    public static final String PROP_QUERY_LOG_FILE = "queryLogFile";

    /**
     * The default value for PROP_QUERY_LOG_FILE.
     */
    public static final String PROP_QUERY_LOG_FILE_DEFAULT = null;

    /**
     * A sphinx property that defines that maxium number of trigrams to be
     * cached
     */
    public static final String PROP_TRIGRAM_CACHE_SIZE = "trigramCacheSize";

    /**
     * The default value for the PROP_TRIGRAM_CACHE_SIZE property
     */
    public static final int PROP_TRIGRAM_CACHE_SIZE_DEFAULT = 100000;

    /**
     * A sphinx property that defines the maximum number of bigrams to be
     * cached.
     */
    public static final String PROP_BIGRAM_CACHE_SIZE = "bigramCacheSize";

    /**
     * The default value for the PROP_BIGRAM_CACHE_SIZE property
     */
    public static final int PROP_BIGRAM_CACHE_SIZE_DEFAULT = 50000;

    /**
     * A sphinx property that controls whether the bigram and trigram caches
     * are cleared after every utterance
     */
    public static final String PROP_CLEAR_CACHES_AFTER_UTTERANCE = "clearCachesAfterUtterance";

    /**
     * The default value for the PROP_CLEAR_CACHES_AFTER_UTTERANCE property
     */
    public static final boolean PROP_CLEAR_CACHES_AFTER_UTTERANCE_DEFAULT = false;

    /**
     * Sphinx property that defines the language weight for the search
     */
    public final static String PROP_LANGUAGE_WEIGHT = "languageWeight";

    /**
     * The default value for the PROP_LANGUAGE_WEIGHT property
     */
    public final static float PROP_LANGUAGE_WEIGHT_DEFAULT = 1.0f;
    /**
     * Sphinx property that defines the logMath component.
     */
    public final static String PROP_LOG_MATH = "logMath";

    /**
     * Sphinx propert that controls whether or not the language model will
     * apply the language weight and word insertion probability
     */
    public final static String PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP = "applyLanguageWeightAndWip";

    /**
     * The default value for PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP
     */
    public final static boolean PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP_DEFAULT = false;

    /**
     * Word insertion probability property
     */
    public final static String PROP_WORD_INSERTION_PROBABILITY = "wordInsertionProbability";

    /**
     * The default value for PROP_WORD_INSERTION_PROBABILITY
     */
    public final static double PROP_WORD_INSERTION_PROBABILITY_DEFAULT = 1.0;

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

    // ------------------------------
    // Configuration data
    // ------------------------------
    private LogMath logMath;
    private String name;
    private String ngramLogFile;
    private int maxTrigramCacheSize;
    private int maxBigramCacheSize;
    private boolean clearCacheAfterUtterance;
    private boolean calculateUnigramSmear = false;
    private int maxDepth;
    private Dictionary dictionary;
    private String format;
    private String location;
    private boolean applyLanguageWeightAndWip;
    private float languageWeight;
    private double wip;
    private float unigramWeight;

    // -------------------------------
    // Statistics
    // -------------------------------
    private int bigramMisses;
    private int trigramMisses;
    private int trigramHit;

    // -------------------------------
    // subcomponents
    // --------------------------------
    private BinaryLoader loader;
    private PrintWriter logFile;

    // -------------------------------
    // Working data
    // --------------------------------
    private Map unigramIDMap;
    private Map loadedTrigramBuffer;
    private LRUCache trigramCache;
    private LRUCache bigramCache;

    private BigramBuffer[] loadedBigramBuffers;
    private UnigramProbability[] unigrams;
    private int[] trigramSegmentTable;
    private float[] bigramProbTable;
    private float[] trigramProbTable;
    private float[] trigramBackoffTable;
    private double[] unigramSmearTerm;

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#register(java.lang.String,
     *      edu.cmu.sphinx.util.props.Registry)
     */
    public void register(String name, Registry registry)
            throws PropertyException {
        this.name = name;
        registry.register(PROP_FORMAT, PropertyType.STRING);
        registry.register(PROP_LOCATION, PropertyType.STRING);
        registry.register(PROP_QUERY_LOG_FILE, PropertyType.STRING);
        registry.register(PROP_TRIGRAM_CACHE_SIZE, PropertyType.INT);
        registry.register(PROP_BIGRAM_CACHE_SIZE, PropertyType.INT);
        registry.register(PROP_CLEAR_CACHES_AFTER_UTTERANCE,
                PropertyType.BOOLEAN);
        registry.register(PROP_MAX_DEPTH, PropertyType.INT);
        registry.register(PROP_LOG_MATH, PropertyType.COMPONENT);
        registry.register(PROP_DICTIONARY, PropertyType.COMPONENT);
        registry.register(PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP,
                PropertyType.BOOLEAN);
        registry.register(PROP_LANGUAGE_WEIGHT, PropertyType.FLOAT);
        registry.register(PROP_WORD_INSERTION_PROBABILITY, PropertyType.DOUBLE);
        registry.register(PROP_UNIGRAM_WEIGHT, PropertyType.FLOAT);
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
     */
    public void newProperties(PropertySheet ps) throws PropertyException {
        format = ps.getString(LanguageModel.PROP_FORMAT,
                LanguageModel.PROP_FORMAT_DEFAULT);
        location = ps.getString(LanguageModel.PROP_LOCATION,
                LanguageModel.PROP_LOCATION_DEFAULT);
        ngramLogFile = ps.getString(PROP_QUERY_LOG_FILE,
                PROP_QUERY_LOG_FILE_DEFAULT);
        maxTrigramCacheSize = ps.getInt(PROP_TRIGRAM_CACHE_SIZE,
                PROP_TRIGRAM_CACHE_SIZE_DEFAULT);
        maxBigramCacheSize = ps.getInt(PROP_BIGRAM_CACHE_SIZE,
                PROP_BIGRAM_CACHE_SIZE_DEFAULT);
        clearCacheAfterUtterance = ps.getBoolean(
                PROP_CLEAR_CACHES_AFTER_UTTERANCE,
                PROP_CLEAR_CACHES_AFTER_UTTERANCE_DEFAULT);
        maxDepth = ps.getInt(LanguageModel.PROP_MAX_DEPTH,
                LanguageModel.PROP_MAX_DEPTH_DEFAULT);
        logMath = (LogMath) ps.getComponent(PROP_LOG_MATH, LogMath.class);
        dictionary = (Dictionary) ps.getComponent(PROP_DICTIONARY,
                Dictionary.class);
        applyLanguageWeightAndWip = ps.getBoolean(
                PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP,
                PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP_DEFAULT);
        languageWeight = ps.getFloat(PROP_LANGUAGE_WEIGHT,
                PROP_LANGUAGE_WEIGHT_DEFAULT);
        wip = ps.getDouble(PROP_WORD_INSERTION_PROBABILITY,
                PROP_WORD_INSERTION_PROBABILITY_DEFAULT);
        unigramWeight = ps.getFloat(PROP_UNIGRAM_WEIGHT,
                PROP_UNIGRAM_WEIGHT_DEFAULT);
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#getName()
     */
    public String getName() {
        return name;
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.linguist.language.ngram.LanguageModel#allocate()
     */
    public void allocate() throws IOException {
        Timer.start("LM Load");
        // create the log file if specified
        if (ngramLogFile != null) {
            logFile = new PrintWriter(new FileOutputStream(ngramLogFile));
        }
        unigramIDMap = new HashMap();
        loadedTrigramBuffer = new HashMap();
        trigramCache = new LRUCache(maxTrigramCacheSize);
        bigramCache = new LRUCache(maxBigramCacheSize);
        loader = new BinaryLoader(format, location, applyLanguageWeightAndWip,
                logMath, languageWeight, wip, unigramWeight);
        unigrams = loader.getUnigrams();
        bigramProbTable = loader.getBigramProbabilities();
        trigramProbTable = loader.getTrigramProbabilities();
        trigramBackoffTable = loader.getTrigramBackoffWeights();
        trigramSegmentTable = loader.getTrigramSegments();
        buildUnigramIDMap(dictionary);
        loadedBigramBuffers = new BigramBuffer[unigrams.length];
        if (maxDepth == LanguageModel.PROP_MAX_DEPTH_DEFAULT) {
            maxDepth = loader.getMaxDepth();
        } else if (maxDepth == 0) {
            throw new Error("Invalid LM max-depth: " + maxDepth);
        }

        System.out.println("# of unigrams: " + loader.getNumberUnigrams());
        System.out.println("# of  bigrams: " + loader.getNumberBigrams());
        System.out.println("# of trigrams: " + loader.getNumberTrigrams());

        if (calculateUnigramSmear) {
            buildSmearInfo();
        }
        Timer.stop("LM Load");

    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.linguist.language.ngram.LanguageModel#deallocate()
     */
    public void deallocate() {
        // TODO write me

    }

    /**
     * Builds the map from unigram to unigramID. Also finds the startWordID and
     * endWordID.
     */
    private void buildUnigramIDMap(Dictionary dictionary) {
        int missingWords = 0;
        String[] words = loader.getWords();
        for (int i = 0; i < words.length; i++) {
            Word word = dictionary.getWord(words[i]);
            if (word == null) {
                missingWords++;
            }
            unigramIDMap.put(word, unigrams[i]);
        }

        if (missingWords > 0) {
            System.out.println("Warning: Dictionary is missing " + missingWords
                    + " words that are contained in the language model.");
        }
    }

    /**
     * Called before a recognition
     */
    public void start() {
        if (logFile != null) {
            logFile.println("<START_UTT>");
        }
    }

    /**
     * Called after a recognition
     */
    public void stop() {
        clearCache();
        if (logFile != null) {
            logFile.println("<END_UTT>");
            logFile.flush();
        }
    }

    /**
     * Clears the various N-gram caches.
     */
    private void clearCache() {
        for (int i = 0; i < loadedBigramBuffers.length; i++) {
            BigramBuffer buffer = loadedBigramBuffers[i];
            if (buffer != null) {
                if (!buffer.getUsed()) {
                    loadedBigramBuffers[i] = null; // free the BigramBuffer
                } else {
                    buffer.setUsed(false);
                }
            }
        }

        // loadedBigramBuffers = new BigramBuffer[unigrams.length];
        loadedTrigramBuffer = new HashMap();

        System.out.println("LM Cache: 3-g " + trigramCache.size() + " 2-g "
                + bigramCache.size());

        if (clearCacheAfterUtterance) {
            trigramCache = new LRUCache(maxTrigramCacheSize);
            bigramCache = new LRUCache(maxBigramCacheSize);
        }
    }

    /**
     * Gets the ngram probability of the word sequence represented by the word
     * list
     * 
     * @param wordSequence
     *                the word sequence
     * 
     * @return the probability of the word sequence. Probability is in logMath
     *         log base
     *  
     */
    public float getProbability(WordSequence wordSequence) {
        if (logFile != null) {
            logFile.println(wordSequence.toText());
        }
        int numberWords = wordSequence.size();
        if (numberWords <= maxDepth) {
            if (numberWords == 3) {
                return getTrigramProbability(wordSequence);
            } else if (numberWords == 2) {
                return getBigramProbability(wordSequence);
            } else if (numberWords == 1) {
                return getUnigramProbability(wordSequence);
            }
        }
        throw new Error("Unsupported N-gram: " + wordSequence.size());
    }

    /**
     * Returns the unigram probability of the given unigram.
     * 
     * @param wordSequence
     *                the unigram word sequence
     * 
     * @return the unigram probability
     */
    private float getUnigramProbability(WordSequence wordSequence) {
        Word unigram = wordSequence.getWord(0);
        UnigramProbability unigramProb = getUnigram(unigram);
        if (unigramProb == null) {
            throw new Error("Unigram not in LM: " + unigram);
        }
        return unigramProb.getLogProbability();
    }

    /**
     * Returns its UnigramProbability if this language model has the given
     * unigram.
     * 
     * @param unigram
     *                the unigram to find
     * 
     * @return the UnigramProbability, or null if this language model does not
     *         have the unigram
     */
    private UnigramProbability getUnigram(Word unigram) {
        return (UnigramProbability) unigramIDMap.get(unigram);
    }

    /**
     * Returns true if this language model has the given unigram.
     * 
     * @param unigram
     *                the unigram to find
     * 
     * @return true if this LM has this unigram, false otherwise
     */
    private boolean hasUnigram(Word unigram) {
        return (unigramIDMap.get(unigram) != null);
    }

    /**
     * Returns the ID of the given word.
     * 
     * @param word
     *                the word to find the ID
     * 
     * @return the ID of the word
     */
    public final int getWordID(Word word) {
        UnigramProbability probability = getUnigram(word);
        if (probability == null) {
            throw new IllegalArgumentException("No word ID: " + word);
        } else {
            return probability.getWordID();
        }
    }

    /**
     * Gets the smear term for the given wordSequence
     * 
     * @param wordSequence
     *                the word sequence
     * @return the smear term associated with this word sequence
     */
    public float getSmear(WordSequence wordSequence) {
        float smearTerm = 0.0f;
        if (calculateUnigramSmear) {
            int length = wordSequence.size();
            if (length > 0) {
                int wordID = getWordID(wordSequence.getWord(length - 1));
                smearTerm = (float) unigramSmearTerm[wordID];
            }
        }
        if (calculateUnigramSmear && true) {
            System.out.println("SmearTerm: " + smearTerm);
        }
        return smearTerm;
    }

    /**
     * Returns the unigram probability of the given unigram.
     * 
     * @param wordSequence
     *                the unigram word sequence
     * 
     * @return the unigram probability
     */
    private float getBigramProbability(WordSequence wordSequence) {

        Word firstWord = wordSequence.getWord(0);
        if (loader.getNumberBigrams() <= 0 || !hasUnigram(firstWord)) {
            return getUnigramProbability(wordSequence.getNewest());
        }

        BigramProbability bigramProbability = findBigram(wordSequence);

        if (bigramProbability != null) {
            return bigramProbTable[bigramProbability.getProbabilityID()];
        } else {
            Word secondWord = wordSequence.getWord(1);

            if (getUnigram(secondWord) == null) {
                throw new Error("Bad word2: " + secondWord);
            }

            // System.out.println("Didn't find bigram");
            int firstWordID = getWordID(firstWord);
            int secondWordID = getWordID(secondWord);
            bigramMisses++;
            return (unigrams[firstWordID].getLogBackoff() + unigrams[secondWordID]
                    .getLogProbability());
        }
    }

    /**
     * Finds the BigramProbability for a particular bigram
     * 
     * @param ws
     *                the word sequence
     * 
     * @return the BigramProbability of the bigram, or null if the given first
     *         word has no bigrams
     */
    private BigramProbability findBigram(WordSequence ws) {

        BigramProbability bigramProbability = (BigramProbability) bigramCache
                .get(ws);

        if (bigramProbability == null) {
            int firstWordID = getWordID(ws.getWord(0));
            int secondWordID = getWordID(ws.getWord(1));

            BigramBuffer bigrams = getBigramBuffer(firstWordID);

            if (bigrams != null) {
                bigrams.setUsed(true);
                bigramProbability = bigrams.findBigram(secondWordID);
                if (bigramProbability != null) {
                    bigramCache.put(ws, bigramProbability);
                }
            }
        }

        return bigramProbability;
    }

    /**
     * Returns the bigrams of the given word
     * 
     * @param firstWordID
     *                the ID of the word
     * 
     * @return the bigrams of the word
     */
    private BigramBuffer getBigramBuffer(int firstWordID) {
        BigramBuffer bigramBuffer = loadedBigramBuffers[firstWordID];
        if (bigramBuffer == null) {
            int numberBigrams = getNumberBigramFollowers(firstWordID);
            if (numberBigrams > 0) {
                bigramBuffer = loadBigramBuffer(firstWordID, numberBigrams);
                if (bigramBuffer != null) {
                    loadedBigramBuffers[firstWordID] = bigramBuffer;
                }
            }
        }
        return bigramBuffer;
    }

    /**
     * Loads the bigram followers of the given first word in a bigram from disk
     * to memory. It actually loads (numberFollowers + 1) bigrams, since we
     * need the first bigram of the next word to determine the number of
     * trigrams of the last bigram.
     * 
     * @param firstWordID
     *                ID of the first word
     * @param numberFollowers
     *                the number of bigram followers this word has
     * 
     * @return the bigram followers of the given word
     */
    private BigramBuffer loadBigramBuffer(int firstWordID, int numberFollowers) {
        BigramBuffer followers = null;
        int firstBigramEntry = unigrams[firstWordID].getFirstBigramEntry();
        int size = (numberFollowers + 1) * BYTES_PER_BIGRAM;
        long position = (long) (loader.getBigramOffset() + (firstBigramEntry * BYTES_PER_BIGRAM));
        try {
            byte[] buffer = loader.loadBuffer(position, size);
            followers = new BigramBuffer(buffer, numberFollowers + 1, loader
                    .getBigEndian());
        } catch (IOException ioe) {
            ioe.printStackTrace();
            throw new Error("Error loading bigram followers");
        }

        return followers;
    }

    /**
     * Returns the number of bigram followers of a word.
     * 
     * @param wordID
     *                the ID of the word
     * 
     * @return the number of bigram followers
     */
    private int getNumberBigramFollowers(int wordID) {
        return unigrams[wordID + 1].getFirstBigramEntry()
                - unigrams[wordID].getFirstBigramEntry();
    }

    /**
     * Returns the language probability of the given trigram.
     * 
     * @param wordSequence
     *                the trigram word sequence
     * 
     * @return the trigram probability
     */
    private float getTrigramProbability(WordSequence wordSequence) {
        Word firstWord = wordSequence.getWord(0);

        if (loader.getNumberTrigrams() == 0 || !hasUnigram(firstWord)) {
            return getBigramProbability(wordSequence.getNewest());
        }

        Float probability = (Float) trigramCache.get(wordSequence);

        if (probability == null) {
            float score = 0.0f;

            int trigramProbID = findTrigram(wordSequence);

            if (trigramProbID != -1) {
                trigramHit++;
                score = trigramProbTable[trigramProbID];
            } else {
                trigramMisses++;
                BigramProbability bigram = findBigram(wordSequence.getOldest());
                if (bigram != null) {
                    score = trigramBackoffTable[bigram.getBackoffID()]
                            + getBigramProbability(wordSequence.getNewest());
                } else {
                    score = getBigramProbability(wordSequence.getNewest());
                }
            }
            probability = new Float(score);
            trigramCache.put(wordSequence, probability);
        }

        return probability.floatValue();
    }

    /**
     * Finds or loads the trigram probability of the given trigram.
     * 
     * @param wordSequence
     *                the trigram to load
     * 
     * @return a TrigramProbability of the given trigram
     */
    private int findTrigram(WordSequence wordSequence) {

        int trigram = -1;

        WordSequence oldest = wordSequence.getOldest();
        TrigramBuffer trigramBuffer = (TrigramBuffer) loadedTrigramBuffer
                .get(oldest);

        if (trigramBuffer == null) {

            int firstWordID = getWordID(wordSequence.getWord(0));
            int secondWordID = getWordID(wordSequence.getWord(1));

            trigramBuffer = loadTrigramBuffer(firstWordID, secondWordID);

            if (trigramBuffer != null) {
                loadedTrigramBuffer.put(oldest, trigramBuffer);
            }
        }

        if (trigramBuffer != null) {
            int thirdWordID = getWordID(wordSequence.getWord(2));
            trigram = trigramBuffer.findProbabilityID(thirdWordID);
        }

        return trigram;
    }

    /**
     * Loads into a buffer all the trigram followers of the given bigram.
     * 
     * @param firstWordID
     *                the ID of the first word
     * @param secondWordID
     *                the ID of the second word
     * 
     * @return a TrigramBuffer of all the trigram followers of the given two
     *         words
     */
    private TrigramBuffer loadTrigramBuffer(int firstWordID, int secondWordID) {
        TrigramBuffer trigramBuffer = null;

        BigramBuffer bigramBuffer = getBigramBuffer(firstWordID);

        if (bigramBuffer != null) {
            BigramProbability bigram = bigramBuffer.findBigram(secondWordID);

            if (bigram != null) {

                BigramProbability nextBigram = bigramBuffer
                        .getBigramProbability(bigram.getWhichFollower() + 1);

                int firstBigramEntry = unigrams[firstWordID]
                        .getFirstBigramEntry();
                int firstTrigramEntry = getFirstTrigramEntry(bigram,
                        firstBigramEntry);
                int numberTrigrams = getFirstTrigramEntry(nextBigram,
                        firstBigramEntry)
                        - firstTrigramEntry;
                int size = numberTrigrams * BYTES_PER_TRIGRAM;
                long position = (loader.getTrigramOffset() + (long) (firstTrigramEntry * BYTES_PER_TRIGRAM));

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
     * @param bigram
     *                the bigram which first trigram entry we're looking for
     * @param firstBigramEntry
     *                the index of the first bigram entry of the bigram in
     *                question
     * 
     * @return the index of the first trigram entry of the given bigram
     */
    private int getFirstTrigramEntry(BigramProbability bigram,
            int firstBigramEntry) {
        int firstTrigramEntry = trigramSegmentTable[(firstBigramEntry + bigram
                .getWhichFollower()) >> loader.getLogBigramSegmentSize()]
                + bigram.getFirstTrigramEntry();
        return firstTrigramEntry;
    }

    /**
     * Returns the backoff probability for the give sequence of words
     * 
     * @param wordSequence
     *                the sequence of words
     * 
     * @return the backoff probability in LogMath log base
     */
    public float getBackoff(WordSequence wordSequence) {
        float logBackoff = 0.0f; // log of 1.0
        UnigramProbability prob = null; //getProb(wordSequence);
        if (prob != null) {
            logBackoff = prob.getLogBackoff();
        }
        return logBackoff;
    }

    /**
     * Returns the maximum depth of the language model
     * 
     * @return the maximum depth of the language model
     */
    public int getMaxDepth() {
        return maxDepth;
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
     * Returns the number of times when a bigram is queried, but there is no
     * bigram in the LM (in which case it uses the backoff probabilities).
     * 
     * @return the number of bigram misses
     */
    public int getBigramMisses() {
        return bigramMisses;
    }

    /**
     * Returns the number of times when a trigram is queried, but there is no
     * trigram in the LM (in which case it uses the backoff probabilities).
     * 
     * @return the number of trigram misses
     */
    public int getTrigramMisses() {
        return trigramMisses;
    }

    /**
     * Returns the number of trigram hits.
     * 
     * @return the number of trigram hits
     */
    public int getTrigramHits() {
        return trigramHit;
    }

    private void buildSmearInfo() {
        double S0 = 0;
        double R0 = 0;

        double[] ugNumerator = new double[unigrams.length];
        double[] ugDenominator = new double[unigrams.length];
        double[] ugAvgLogProb = new double[unigrams.length];
        unigramSmearTerm = new double[unigrams.length];

        for (int i = 0; i < unigrams.length; i++) {
            float p = unigrams[i].getLogProbability();
            S0 += p;
            R0 += logMath.logToLinear(p) * p;
        }

        for (int i = 0; i < loadedBigramBuffers.length; i++) {
            System.out.println("bi " + i);
            BigramBuffer bigram = getBigramBuffer(i);
            if (bigram == null) {
                unigramSmearTerm[i] = 1.0;
                continue;
            }

            ugNumerator[i] = 0.0;
            ugDenominator[i] = 0.0;
            ugAvgLogProb[i] = 0.0;
            for (int j = 0; j < bigram.getNumberNGrams(); j++) {
                int wordID = bigram.getWordID(j);
                BigramProbability bgProb = bigram.getBigramProbability(j);
                float logugprob = unigrams[wordID].getLogProbability();
                float logbgprob = bigramProbTable[bgProb.getProbabilityID()];
                float logbackoffbgprob = unigrams[i].getLogBackoff()
                        * logugprob;
                double bgprob = logMath.logToLinear(logbgprob);
                double backoffbgprob = logMath.logToLinear(logbackoffbgprob);

                ugNumerator[i] += (bgprob * logbgprob - backoffbgprob
                        * logbackoffbgprob)
                        * logugprob;

                ugDenominator[i] += (bgprob - backoffbgprob) * logugprob;
            }

            double backoff = logMath.logToLinear(unigrams[i].getLogBackoff());
            double logBackoff = unigrams[i].getLogBackoff();
            ugNumerator[i] += backoff * (logBackoff * S0 + R0);
            ugAvgLogProb[i] = ugDenominator[i] + backoff * S0;
            unigramSmearTerm[i] = ugNumerator[i] / ugDenominator[i];
        }

        /*
         * for (i = 0; i < vocabsize; i++) { for (j=0; j <word[i].num_bigrams;
         * j++) { bigram = word[i].bigram[j]; k = bigram.wordid; if
         * (bigram.num_trigrams == 0) { bigram.ug_smear =
         * word[wordid].ug_smear; }
         * 
         * bg_numerator = 0; bg_denominator = 0; for (l=0; l
         * <bigram.num_trigrams; l++) { m = bigram.trigram[l].wordid; tgprob =
         * bigram.trigramprob[l].bgprob = BIGRAMPROB(k, m); ugprob =
         * word[m].ugprob; bgprob;
         * 
         * log(tgprob) log(backofftgprob)) * log(ugprob);
         * 
         * bg_denominator += (tgprob - backofftgprob) log(ugprob) *
         * log(ugprob); } (log(bigram.bgbackoff)* ugavglogprob[k] -
         * ugnumerator[k]);
         * 
         * ugdenominator[k]; bigram.ugsmear = bg_numerator / bg_denominator; } }
         */
    }

}

/**
 * An LRU cache
 */

class LRUCache extends LinkedHashMap {
    int maxSize;

    /**
     * Creates an LRU cache with the given maximum size
     * 
     * @param maxSize
     *                the maximum size of the cache
     */
    LRUCache(int maxSize) {
        this.maxSize = maxSize;
    }

    /**
     * Determines if the eldest entry in the map should be removed.
     * 
     * @param eldest
     *                the eldest entry
     * 
     * @return true if the eldest entry should be removed
     */
    protected boolean removeEldestEntry(Map.Entry eldest) {
        return size() > maxSize;
    }
}
