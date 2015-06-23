package edu.cmu.sphinx.linguist.language.ngram.trie;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.URL;
import java.util.Set;
import java.util.logging.Logger;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Dictionary;
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

public class NgramTrie implements LanguageModel {

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
    protected NgramTrieQuant quant;    
    
    public NgramTrie(String format, URL location, String ngramLogFile,
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

    public NgramTrie() {
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
        
        TimerPool.getTimer(this, "Load LM").stop();
    }

	@Override
	public void deallocate() throws IOException {
	}

	@Override
	public float getProbability(WordSequence wordSequence) {
		return 0;
	}

	@Override
	public float getSmear(WordSequence wordSequence) {
		return 0;
	}

	@Override
	public Set<String> getVocabulary() {
		return null;
	}

	@Override
	public int getMaxDepth() {
		return 0;
	}
	
    public static class TrieUnigram {
        public float prob;
        public float backoff;
        public int next;
    }

}
