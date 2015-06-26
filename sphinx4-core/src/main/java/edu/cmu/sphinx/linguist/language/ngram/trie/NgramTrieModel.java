package edu.cmu.sphinx.linguist.language.ngram.trie;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.URL;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.logging.Level;
import java.util.logging.Logger;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Dictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.LanguageModel;
import edu.cmu.sphinx.linguist.language.ngram.trie.BinaryLoader;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.TimerPool;
import edu.cmu.sphinx.util.props.ConfigurationManagerUtils;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.S4Boolean;
import edu.cmu.sphinx.util.props.S4Double;
import edu.cmu.sphinx.util.props.S4String;

public class NgramTrieModel implements LanguageModel {

    /**
     * The property for the name of the file that logs all the queried N-grams.
     * If this property is set to null, it means that the queried N-grams are
     * not logged.
     */
    @S4String(mandatory = false)
    public static final String PROP_QUERY_LOG_FILE = "queryLogFile";

    /** The property that defines the language weight for the search */
    @S4Double(defaultValue = 1.0f)
    public final static String PROP_LANGUAGE_WEIGHT = "languageWeight";

    /**
     * The property that controls whether or not the language model will apply
     * the language weight and word insertion probability
     */
    @S4Boolean(defaultValue = false)
    public final static String PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP = "applyLanguageWeightAndWip";

    /** Word insertion probability property */
    @S4Double(defaultValue = 1.0f)
    public final static String PROP_WORD_INSERTION_PROBABILITY = "wordInsertionProbability";

    // ------------------------------
    // Configuration data
    // ------------------------------
    URL location;
    protected Logger logger;
    protected LogMath logMath;
    protected int maxDepth;
    protected int curDepth;
    protected int[] counts;

    protected Dictionary dictionary;
    protected String format;
    protected boolean applyLanguageWeightAndWip;
    protected float languageWeight;
    protected float unigramWeight;
    protected double wip;

    // -------------------------------
    // Statistics
    // -------------------------------
    protected String ngramLogFile;

    // -------------------------------
    // subcomponents
    // --------------------------------
    //private BinaryLoader loader;
    private PrintWriter logFile;

    //-----------------------------
    // Trie structure
    //-----------------------------
    protected TrieUnigram[] unigrams;
    protected Map<Word, Integer> unigramIDMap;
    protected NgramTrieQuant quant;
    protected NgramTrie trie;
    
    public NgramTrieModel(String format, URL location, String ngramLogFile,
            int maxDepth, Dictionary dictionary,
            boolean applyLanguageWeightAndWip, float languageWeight,
            double wip, float unigramWeight) {
        logger = Logger.getLogger(getClass().getName());
        this.format = format;
        this.location = location;
        this.ngramLogFile = ngramLogFile;
        this.maxDepth = maxDepth;
        logMath = LogMath.getLogMath();
        this.dictionary = dictionary;
        this.applyLanguageWeightAndWip = applyLanguageWeightAndWip;
        this.languageWeight = languageWeight;
        this.wip = wip;
        this.unigramWeight = unigramWeight;
    }

    public NgramTrieModel() {
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.
     * util.props.PropertySheet)
     */
    @Override
    public void newProperties(PropertySheet ps) throws PropertyException {
        logger = ps.getLogger();
        location = ConfigurationManagerUtils.getResource(PROP_LOCATION, ps);
        ngramLogFile = ps.getString(PROP_QUERY_LOG_FILE);
        maxDepth = ps.getInt(LanguageModel.PROP_MAX_DEPTH);
        dictionary = (Dictionary) ps.getComponent(PROP_DICTIONARY);
        applyLanguageWeightAndWip = ps
                .getBoolean(PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP);
        languageWeight = ps.getFloat(PROP_LANGUAGE_WEIGHT);
        wip = ps.getDouble(PROP_WORD_INSERTION_PROBABILITY);
        unigramWeight = ps.getFloat(PROP_UNIGRAM_WEIGHT);
    }

    /**
     * Builds the map from unigram to unigramID. Also finds the startWordID and
     * endWordID.
     * 
     * @param dictionary
     * */
    private void buildUnigramIDMap(Dictionary dictionary, String[] words) {
        int missingWords = 0;
        if (unigramIDMap == null)
            unigramIDMap = new HashMap<Word, Integer>();
        for (int i = 0; i < words.length; i++) {
            Word word = dictionary.getWord(words[i]);
            if (word == null) {
                logger.warning("The dictionary is missing a phonetic transcription for the word '"
                        + words[i] + "'");
                missingWords++;
            }

            unigramIDMap.put(word, i);

            if (logger.isLoggable(Level.FINE))
                logger.fine("Word: " + word);
        }

        if (missingWords > 0)
            logger.warning("Dictionary is missing " + missingWords
                    + " words that are contained in the language model.");
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.linguist.language.ngram.LanguageModel#allocate()
     */
    //@SuppressWarnings("unchecked")
    public void allocate() throws IOException {
        TimerPool.getTimer(this, "Load LM").start();

        logger.info("Loading n-gram language model from: " + location);

        // create the log file if specified
        if (ngramLogFile != null)
            logFile = new PrintWriter(new FileOutputStream(ngramLogFile));
        BinaryLoader loader;
        try {
            loader = new BinaryLoader(new File(location.toURI()));
        } catch (Exception ex) {
        	loader = new BinaryLoader(new File(location.getPath()));
        }
        loader.verifyHeader();
        counts = loader.readCounts();
        if (maxDepth <= 0 || maxDepth > counts.length)
            maxDepth = counts.length;
        if (maxDepth > 1) {
            quant = loader.readQuant(maxDepth);
        }
        unigrams = loader.readUnigrams(counts[0]);
        if (maxDepth > 1) {
            trie = new NgramTrie(counts, quant.getProbBoSize(), quant.getProbSize());
            loader.readByteArr(trie.getMem());
        }
        //string words can be read here
        String[] words = loader.readWords(counts[0]);
        buildUnigramIDMap(dictionary, words);
        loader.close();
        TimerPool.getTimer(this, "Load LM").stop();
    }

    @Override
    public void deallocate() throws IOException {
    }

    private float getAvailableProb(WordSequence wordSequence, TrieRange range, float prob) {
        if (!range.isValid()) return prob;
        for (int reverseOrderMinusTwo = wordSequence.size() - 2; reverseOrderMinusTwo >= 0; reverseOrderMinusTwo--) {
            int orderMinusTwo = wordSequence.size() - 2 - reverseOrderMinusTwo;
            if (orderMinusTwo + 1 == maxDepth) break;
            int wordId = unigramIDMap.get(wordSequence.getWord(reverseOrderMinusTwo));
            float updatedProb = trie.readNgramProb(wordId, orderMinusTwo, range, quant);
            if (!range.isValid()) break;
            prob = updatedProb;
            curDepth++;
        }
        return prob;
    }

    private float getAvailableBackoff(WordSequence wordSequence) {
        float backoff = 0.0f;
        int wordsNum = wordSequence.size();
        int wordId = unigramIDMap.get(wordSequence.getWord(wordsNum - 2));
        TrieRange range = new TrieRange(unigrams[wordId].next, unigrams[wordId + 1].next);
        if (curDepth == 1) {
            backoff += unigrams[wordId].backoff;
            curDepth = 2;
        }
        int sequenceIdx, orderMinusTwo;
        for (sequenceIdx = wordsNum - curDepth - 1, orderMinusTwo = 0; sequenceIdx >= 0; sequenceIdx--, orderMinusTwo++) {
            int tmpWordId = unigramIDMap.get(wordSequence.getWord(sequenceIdx));
            float tmpBackoff = trie.readNgramBackoff(tmpWordId, orderMinusTwo, range, quant);
            if (!range.isValid()) break;
            backoff += tmpBackoff;
        }
        return backoff;
    }

    /**
     * extracts word sequence probability without using caching, 
     * making fresh LM trie traversing
     * @param wordSequence - sequence of words to get probability for
     * @return probability of specialized sequence of words
     */
    private float getContextlessProbability(WordSequence wordSequence) {
        int wordsNum = wordSequence.size();
        int wordId = unigramIDMap.get(wordSequence.getWord(wordsNum - 1));
        TrieRange range = new TrieRange(unigrams[wordId].next, unigrams[wordId + 1].next);
        float prob = unigrams[wordId].prob;
        curDepth = 1;
        if (wordsNum == 1)
            return prob;
        //find prob of ngrams of higher order if any
        prob = getAvailableProb(wordSequence, range, prob);
        if (curDepth < wordsNum) {
            //use backoff for rest of ngram
            prob += getAvailableBackoff(wordSequence);
        }
        return prob;
    }

    @Override
    public float getProbability(WordSequence wordSequence) {
        int numberWords = wordSequence.size();

        if (numberWords > maxDepth) {
            throw new Error("Unsupported NGram: " + wordSequence.size());
        }

        //cap
        return getContextlessProbability(wordSequence);
    }

    @Override
    public float getSmear(WordSequence wordSequence) {
        return 0;
    }

	@Override
    public Set<String> getVocabulary() {
        Set<String> vocabulary = new HashSet<String>();
        if (unigramIDMap != null) {
            for (Word word : unigramIDMap.keySet())
                vocabulary.add(word.toString());
        }
        return Collections.unmodifiableSet(vocabulary);
    }

    @Override
    public int getMaxDepth() {
        return maxDepth;
    }

    public static class TrieUnigram {
        public float prob;
        public float backoff;
        public int next;
    }

    public static class TrieRange {
        int begin;
        int end;
        boolean isValid;
        TrieRange(int begin, int end) {
            this.begin = begin;
            this.end = end;
            isValid = true;
        }

        int width() {
            return end - begin;
        }

        void setInvalid() {
            isValid = false;
        }

        boolean isValid() {
            return isValid && begin != end;
        }

    }

}
