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

import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.Utilities;

import java.io.BufferedInputStream;
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.Reader;

import java.net.URL;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;

import java.nio.channels.FileChannel;



/**
 * Reads a binary language model file generated by the 
 * CMU-Cambridge Statistical Language Modelling Toolkit.
 * 
 * Note that all probabilites in the grammar are stored in LogMath log
 * base format. Language Probabilties in the language model file are
 * stored in log 10  base. They are converted to the LogMath logbase.
 */
class BinaryLoader {

    private static final String DARPA_LM_HEADER = "Darpa Trigram LM";

    private static final int LOG2_BIGRAM_SEGMENT_SIZE_DEFAULT = 9;

    private static final float MIN_PROBABILITY = -99.0f;

    private static final String PROP_PREFIX = 
    "edu.cmu.sphinx.knowledge.language.large.BinaryLoader.";
    
    /**
     * Sphinx property for whether to apply the language weight and
     * word insertion probability.
     */
    public static final String PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP =
    PROP_PREFIX + "applyLanguageWeightAndWip";
        
    /**
     * Default value for PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP.
     */
    public static final boolean PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP_DEFAULT =
    false;


    private SphinxProperties props;
    private LogMath logMath;
    private int maxNGram = 3;

    private int bytesRead = 0;

    private UnigramProbability[] unigrams;
    private String[] words;

    private int bigramOffset;
    private int trigramOffset;
    private int numberUnigrams;
    private int numberBigrams;
    private int numberTrigrams;
    private int logBigramSegmentSize;

    private int[] trigramSegmentTable;

    private float[] bigramProbTable;
    private float[] trigramBackoffTable;
    private float[] trigramProbTable;

    private FileInputStream is;
    private FileChannel fileChannel;
    private boolean bigEndian = true;
    private boolean applyLanguageWeightAndWip;

    
    /**
     * Creates a simple ngram model from the data at the URL. The
     * data should be an ARPA format
     *
     * @param context the context for this model
     *
     * @throws IOException if there is trouble loading the data
     */
    public BinaryLoader(String context) 
        throws IOException, FileNotFoundException {
	initialize(context);
    }


    /**
     * Returns the number of unigrams
     *
     * @return the nubmer of unigrams
     */
    public int getNumberUnigrams() {
        return numberUnigrams;
    }


    /**
     * Returns the number of bigrams
     *
     * @return the nubmer of bigrams
     */
    public int getNumberBigrams() {
        return numberBigrams;
    }


    /**
     * Returns the number of trigrams
     *
     * @return the nubmer of trigrams
     */
    public int getNumberTrigrams() {
        return numberTrigrams;
    }


    /**
     * Returns all the unigrams
     *
     * @return all the unigrams
     */
    public UnigramProbability[] getUnigrams() {
        return unigrams;
    }


    /**
     * Returns all the bigram probabilities.
     *
     * @return all the bigram probabilities
     */
    public float[] getBigramProbabilities() {
        return bigramProbTable;
    }


    /**
     * Returns all the trigram probabilities.
     *
     * @return all the trigram probabilities
     */
    public float[] getTrigramProbabilities() {
        return trigramProbTable;
    }


    /**
     * Returns all the trigram backoff weights
     *
     * @return all the trigram backoff weights
     */
    public float[] getTrigramBackoffWeights() {
        return trigramBackoffTable;
    }


    /**
     * Returns the trigram segment table.
     *
     * @return the trigram segment table
     */
    public int[] getTrigramSegments() {
        return trigramSegmentTable;
    }


    /**
     * Returns the log of the bigram segment size
     *
     * @return the log of the bigram segment size
     */
    public int getLogBigramSegmentSize() {
        return logBigramSegmentSize;
    }


    /**
     * Returns all the words.
     *
     * @return all the words
     */
    public String[] getWords() {
        return words;
    }


    /**
     * Initializes this LanguageModel
     *
     * @param context the context to associate this linguist with
     */
    private void initialize(String context) throws IOException {
        this.props = SphinxProperties.getSphinxProperties(context);
        
        String format = props.getString
            (LanguageModel.PROP_FORMAT, LanguageModel.PROP_FORMAT_DEFAULT);
        String location = props.getString
            (LanguageModel.PROP_LOCATION, LanguageModel.PROP_LOCATION_DEFAULT);

        applyLanguageWeightAndWip = props.getBoolean
            (PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP,
             PROP_APPLY_LANGUAGE_WEIGHT_AND_WIP_DEFAULT);
        
        logMath = LogMath.getLogMath(context);
        loadBinary(location);
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
     * Returns the location (or offset) into the file where bigrams start.
     *
     * @return the location of the bigrams
     */
    public int getBigramOffset() {
        return bigramOffset;
    }


    /**
     * Returns the location (or offset) into the file where trigrams start.
     *
     * @return the location of the trigrams
     */
    public int getTrigramOffset() {
        return trigramOffset;
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
     * Loads the contents of the memory-mapped file starting at the 
     * given position and for the given size, into a byte buffer.
     * This method is implemented because MappedByteBuffer.load()
     * does not work properly.
     *
     * @param position the starting position in the file
     * @param size the number of bytes to load
     *
     * @return the loaded ByteBuffer
     */
    public ByteBuffer loadBuffer(long position, int size) 
	throws IOException {
	assert ((position + size) <= fileChannel.size());
        ByteBuffer bb = ByteBuffer.allocate(size);
        fileChannel.position(position);
        int bytesRead = fileChannel.read(bb);
	if (!bigEndian) {
	    bb.order(ByteOrder.LITTLE_ENDIAN);
	}
        if (bytesRead != size) {
            throw new IOException("Insufficient bytes read.");
        }
        return bb;
    }


    /**
     * Loads the bigram at the given absolute index into the bigram region.
     *
     * @param index the absolute index into the bigram region
     *
     * @return a ByteBuffer of the requested bigram
     */
    public BigramBuffer loadBigram(int index) throws IOException {
        long position = (long) bigramOffset + 
            (index * LargeTrigramModel.BYTES_PER_BIGRAM);
        ByteBuffer buffer = loadBuffer
            (position, LargeTrigramModel.BYTES_PER_BIGRAM);
        return (new BigramBuffer(buffer, 1));
    }


    /**
     * Loads the language model from the given location. 
     *
     * @param location the location of the language model
     */
    private void loadBinary(String location) throws IOException {

        FileInputStream fis = new FileInputStream(location);
	DataInputStream stream = new DataInputStream
            (new BufferedInputStream(fis));
	
	// read standard header string-size; set bigEndian flag
	
	int headerLength = readInt(stream, bigEndian);

	if (headerLength != (DARPA_LM_HEADER.length() + 1)) { // not big-endian
	    headerLength = Utilities.swapInteger(headerLength);
	    if (headerLength == (DARPA_LM_HEADER.length() + 1)) {
		bigEndian = false;
                System.out.println("Little-endian");
	    } else {
		throw new Error
		    ("Bad binary LM file magic number: " + headerLength +
		     ", not an LM dumpfile?");
	    }
	} else {
            System.out.println("Big-endian");
        }

	// read and verify standard header string

	String header = readString(stream, headerLength - 1);
	readByte(stream); // read the '\0'
        
	if (!header.equals(DARPA_LM_HEADER)) {
	    throw new Error("Bad binary LM file header: " + header);
	}

	// read LM filename string size and string
	
	int fileNameLength = readInt(stream, bigEndian);
	for (int i = 0; i < fileNameLength; i++) {
	    readByte(stream);
	}

	numberUnigrams = 0;
	logBigramSegmentSize = LOG2_BIGRAM_SEGMENT_SIZE_DEFAULT;
	
	// read version number, if present. it must be <= 0.

	int version = readInt(stream, bigEndian);
	System.out.println("Version: " + version);
	if (version <= 0) { // yes, its the version number
	    readInt(stream, bigEndian); // read and skip timestamp
	    
	    // read and skip format description
	    int formatLength;
	    for (;;) {
		if ((formatLength = readInt(stream, bigEndian)) == 0) {
		    break;
		}
		for (int i = 0; i < formatLength; i++) {
		    readByte(stream);
                }
	    }

	    // read log bigram segment size if present
	    if (version <= -2) {
		logBigramSegmentSize = readInt(stream, bigEndian);
		if (logBigramSegmentSize < 1 || logBigramSegmentSize > 15) {
		    throw new Error("log2(bg_seg_sz) outside range 1..15");
		}
	    }

	    numberUnigrams = readInt(stream, bigEndian);
	} else {
	    numberUnigrams = version;
	}

        int bigramSegmentSize = 1 << logBigramSegmentSize;

	if (numberUnigrams <= 0) {
	    throw new Error("Bad number of unigrams: " + numberUnigrams +
			    ", must be > 0.");
	}
	System.out.println("# of unigrams: " + numberUnigrams);

	numberBigrams = readInt(stream, bigEndian);
	if (numberBigrams < 0) {
	    throw new Error("Bad number of bigrams: " + numberBigrams);
	}
	System.out.println("# of bigrams: " + numberBigrams);

	numberTrigrams = readInt(stream, bigEndian);
	if (numberTrigrams < 0) {
	    throw new Error("Bad number of trigrams: " + numberTrigrams);
	}
	System.out.println("# of trigrams: " + numberTrigrams);

	unigrams = readUnigrams(stream, numberUnigrams+1, bigEndian);


	// skip all the bigram entries, the +1 is the sentinel at the end
	if (numberBigrams > 0) {
            bigramOffset = bytesRead;
            int bytesToSkip = (numberBigrams + 1) * 
                LargeTrigramModel.BYTES_PER_BIGRAM;
	    
            stream.skipBytes(bytesToSkip);
            bytesRead += bytesToSkip;
	}

	// skip all the trigram entries
	if (numberTrigrams > 0) {
            trigramOffset = bytesRead;
            int bytesToSkip = numberTrigrams * 
                LargeTrigramModel.BYTES_PER_TRIGRAM;
	    
            stream.skipBytes(bytesToSkip);
            bytesRead += bytesToSkip;
	}

	// read the bigram probabilities table
	if (numberBigrams > 0) {
            this.bigramProbTable = readFloatTable(stream, bigEndian);
	}

	// read the trigram backoff weight table and trigram prob table
	if (numberTrigrams > 0) {
	    trigramBackoffTable = readFloatTable(stream, bigEndian);
	    trigramProbTable = readFloatTable(stream, bigEndian);
            int trigramSegTableSize = ((numberBigrams+1)/bigramSegmentSize)+1;
            trigramSegmentTable = readIntTable(stream, bigEndian, 
                                               trigramSegTableSize);
        }

	// read word string names
        int wordsStringLength = readInt(stream, bigEndian);
        if (wordsStringLength <= 0) {
            throw new Error("Bad word string size: " + wordsStringLength);
        }

        // read the string of all words
        String wordsString = 
            readString(stream, wordsStringLength).toLowerCase();

        // first make sure string just read contains ucount words
        int numberWords = 0;
        for (int i = 0; i < wordsStringLength; i++) {
            if (wordsString.charAt(i) == '\0') {
                numberWords++;
            }
        }
        if (numberWords != numberUnigrams) {
            throw new Error("Bad # of words: " + numberWords);
        }

        // break up string just read into words
        this.words = wordsString.split("\0");

        applyUnigramWeight();

        if (applyLanguageWeightAndWip) {
            applyLanguageWeightAndWip();
        }

        fis.close();
        stream.close();

        is = new FileInputStream(location);
        fileChannel = is.getChannel();
    }
    
    
    /**
     * Apply the unigram weight to the set of unigrams
     */
    private void applyUnigramWeight() {

        float unigramWeight = props.getFloat
            (LanguageModel.PROP_UNIGRAM_WEIGHT, 
	     LanguageModel.PROP_UNIGRAM_WEIGHT_DEFAULT);

        float logUnigramWeight = logMath.linearToLog(unigramWeight);
        float logNotUnigramWeight = logMath.linearToLog(1.0f - unigramWeight);
        float logUniform = logMath.linearToLog(1.0f/(numberUnigrams));

        float p2 = logUniform + logNotUnigramWeight;

        for (int i = 0; i < numberUnigrams; i++) {
            if (!words[i].equals(Dictionary.SENTENCE_START_SPELLING)) {
                float p1 = unigrams[i].getLogProbability() + logUnigramWeight;
                unigrams[i].setLogProbability(logMath.addAsLinear(p1, p2));
            }
        }
    }


    /**
     * Applies the language weight and the word insertion probability to 
     * the probabilities and backoff weights.
     */
    private void applyLanguageWeightAndWip() {

        float languageWeight = 9.5f;
        float wip = logMath.linearToLog(0.7);
        
        // apply to the unigram probabilities
        for (int i = 0; i < numberUnigrams; i++) {
            UnigramProbability unigram = unigrams[i];
            unigram.setLogProbability
                (unigram.getLogProbability() * languageWeight + wip);
            unigram.setLogBackoff(unigram.getLogBackoff() * languageWeight);
        }

        // apply to the bigram probabilities
        for (int i = 0; i < bigramProbTable.length; i++) {
            bigramProbTable[i] = bigramProbTable[i] * languageWeight + wip;
        }

        // apply to the trigram probabilities
        for (int i = 0; i < trigramProbTable.length; i++) {
            trigramProbTable[i] = trigramProbTable[i] * languageWeight + wip;
        }

        // apply to the trigram backoff weights
        for (int i = 0; i < trigramBackoffTable.length; i++) {
            trigramBackoffTable[i] = trigramBackoffTable[i] * languageWeight;
        }
    }


    /**
     * Reads the probability table from the given DataInputStream.
     *
     * @param stream the DataInputStream from which to read the table
     * @param bigEndian true if the given stream is bigEndian, false otherwise
     */
    private float[] readFloatTable(DataInputStream stream,
                                   boolean bigEndian) throws IOException {
        
	int numProbs = readInt(stream, bigEndian);
	if (numProbs <= 0 || numProbs > 65536) {
	    throw new Error("Bad probabilities table size: " + numProbs);
	}
	float[] probTable = new float[numProbs];
	for (int i = 0; i < numProbs; i++) {
	    probTable[i] = logMath.log10ToLog(readFloat(stream, bigEndian));
	}
        
	return probTable;
    }


    /**
     * Reads a table of integers from the given DataInputStream.
     *
     * @param stream the DataInputStream from which to read the table
     * @param bigEndian true if the given stream is bigEndian, false otherwise
     * @param tableSize the size of the trigram segment table
     *
     * @return the trigram segment table, which is an array of integers
     */
    private int[] readIntTable(DataInputStream stream, 
                               boolean bigEndian, int tableSize) 
        throws IOException {
        int numSegments = readInt(stream, bigEndian);
	if (numSegments != tableSize) {
	    throw new Error("Bad trigram seg table size: " + numSegments);
	}
	int[] segmentTable = new int[numSegments];
	for (int i = 0; i < numSegments; i++) {
	    segmentTable[i] = readInt(stream, bigEndian);
	}
	return segmentTable;
    }


    /**
     * Read in the unigrams in the given DataInputStream.
     *
     * @param stream the DataInputStream to read from
     * @param numberUnigrams the number of unigrams to read
     * @param bigEndian true if the DataInputStream is big-endian,
     *                  false otherwise
     *
     * @return an array of UnigramProbability index by the unigram ID
     */
    private UnigramProbability[] readUnigrams(DataInputStream stream, 
                                              int numberUnigrams, 
                                              boolean bigEndian)
    throws IOException {

        UnigramProbability[] unigrams = new UnigramProbability[numberUnigrams];
                                  
	for (int i = 0; i < numberUnigrams; i++) {

	    // read unigram ID, unigram probability, unigram backoff weight
	    int unigramID = readInt(stream, bigEndian);

            // if we're not reading the sentinel unigram at the end,
            // make sure that the unigram IDs are consecutive
            if (i != (numberUnigrams - 1)) {
                assert (unigramID == i);
            }
            
            float unigramProbability = readFloat(stream, bigEndian);
	    float unigramBackoff = readFloat(stream, bigEndian);
	    int firstBigramEntry = readInt(stream, bigEndian);

            float logProbability = logMath.log10ToLog(unigramProbability);
            float logBackoff = logMath.log10ToLog(unigramBackoff);

            unigrams[i] = new UnigramProbability
                (logProbability, logBackoff, firstBigramEntry);
	}

        return unigrams;
    }


    /**
     * Reads a byte from the given DataInputStream.
     *
     * @param stream the DataInputStream to read from
     *
     * @return the byte read
     */
    private final byte readByte(DataInputStream stream) throws IOException {
        bytesRead++;
        return stream.readByte();
    }


    /**
     * Reads an integer from the given DataInputStream.
     *
     * @param stream the DataInputStream to read from
     * @param bigEndian true if the DataInputStream is in bigEndian,
     *                  false otherwise
     *
     * @return the integer read
     */
    private final int readInt(DataInputStream stream, boolean bigEndian) 
    throws IOException {
        bytesRead += 4;
        if (bigEndian) {
            return stream.readInt();
        } else {
            return Utilities.readLittleEndianInt(stream);
	}
    }


    /**
     * Reads a float from the given DataInputStream.
     *
     * @param stream the DataInputStream to read from
     * @param bigEndian true if the DataInputStream is in bigEndian,
     *                  false otherwise
     *
     * @return the float read
     */
    private final float readFloat(DataInputStream stream, boolean bigEndian)
    throws IOException {
        bytesRead += 4;
        if (bigEndian) {
            return stream.readFloat();
        } else {
            return Utilities.readLittleEndianFloat(stream);
	}
    }


    /**
     * Reads a string of the given length from the given DataInputStream.
     * It is assumed that the DataInputStream contains 8-bit chars.
     *
     * @param stream the DataInputStream to read from
     * @param length the number of characters in the returned string
     *
     * @return a string of the given length from the given DataInputStream
     */
    private final String readString(DataInputStream stream, int length)
        throws IOException {
        StringBuffer buffer = new StringBuffer();
        for (int i = 0; i < length; i++) {
            buffer.append((char)readByte(stream));
	}
        return buffer.toString();
    }
}

