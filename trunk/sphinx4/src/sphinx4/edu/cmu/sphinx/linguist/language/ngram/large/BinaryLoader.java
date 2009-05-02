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

package edu.cmu.sphinx.linguist.language.ngram.large;

import edu.cmu.sphinx.linguist.dictionary.Dictionary;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.Utilities;

import java.io.*;

/**
 * Reads a binary language model file generated by the CMU-Cambridge Statistical Language Modelling Toolkit.
 * <p/>
 * Note that all probabilites in the grammar are stored in LogMath log base format. Language Probabilties in the
 * language model file are stored in log 10 base. They are converted to the LogMath logbase.
 */
public class BinaryLoader {

    private static final String DARPA_LM_HEADER = "Darpa Trigram LM";
    private static final int LOG2_BIGRAM_SEGMENT_SIZE_DEFAULT = 9;
    private static final float MIN_PROBABILITY = -99.0f;
    private static final int MAX_PROB_TABLE_SIZE = 65536;

    private LogMath logMath;
    private int maxNGram;
    private float unigramWeight;
    private float languageWeight;
    private double wip;
    private boolean bigEndian = true;
    private boolean applyLanguageWeightAndWip;

    private int bytesRead = 0;

    private UnigramProbability[] unigrams;
    private String[] words;
    private int bigramOffset;
    private int trigramOffset;
    private int numberUnigrams;
    private int numberBigrams;
    private int numberTrigrams;
    private int logBigramSegmentSize;
    private int startWordID;
    private int endWordID;
    private int[] trigramSegmentTable;

    private float[] bigramProbTable;
    private float[] trigramBackoffTable;
    private float[] trigramProbTable;

    private RandomAccessFile file;


    /**
     * Initializes the binary loader
     *
     * @param format                    the file format
     * @param location                  the location of the model
     * @param applyLanguageWeightAndWip if true apply lw and wip
     * @param logMath                   the logmath to sue
     * @param languageWeight            the language weight
     * @param wip                       the word insertion probability
     * @param unigramWeight             the unigram weight
     * @throws IOException if an I/O error occurs
     */
    public BinaryLoader(String format, File location,
                        boolean applyLanguageWeightAndWip, LogMath logMath,
                        float languageWeight, double wip, float unigramWeight)
            throws IOException {
        startWordID = -1;
        endWordID = -1;
        this.applyLanguageWeightAndWip = applyLanguageWeightAndWip;
        this.logMath = logMath;
        this.languageWeight = languageWeight;
        this.wip = wip;
        this.unigramWeight = unigramWeight;
        loadBinary(location);
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
     * @param context
     *                the context to associate this linguist with
     */


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
     * Returns true if the loaded file is in big-endian.
     *
     * @return true if the loaded file is big-endian
     */
    public boolean getBigEndian() {
        return bigEndian;
    }


    /**
     * Loads the contents of the memory-mapped file starting at the given position and for the given size, into a byte
     * buffer. This method is implemented because MappedByteBuffer.load() does not work properly.
     *
     * @param position the starting position in the file
     * @param size     the number of bytes to load
     * @return the loaded ByteBuffer
     */
    public byte[] loadBuffer(long position, int size) throws IOException {
        // assert ((position + size) <= fileChannel.size());
        file.seek(position);
        byte[] bytes = new byte[size];
        if (file.read(bytes) != size) {
            throw new IOException("Incorrect number of bytes read.");
        }
        return bytes;
    }


    /**
     * Loads the language model from the given file.
     *
     * @param location the file containing the language model
     */
    private void loadBinary(File location) throws IOException {

        DataInputStream stream = new DataInputStream
                (new BufferedInputStream(new FileInputStream(location)));

        // read standard header string-size; set bigEndian flag
        readHeader(stream);

        // +1 is the sentinel unigram at the end
        unigrams = readUnigrams(stream, numberUnigrams + 1, bigEndian);

        skipBigramsTrigrams(stream);

        // read the bigram probabilities table
        if (numberBigrams > 0) {
            this.bigramProbTable = readFloatTable(stream, bigEndian);
        }

        // read the trigram backoff weight table and trigram prob table
        if (numberTrigrams > 0) {
            trigramBackoffTable = readFloatTable(stream, bigEndian);
            trigramProbTable = readFloatTable(stream, bigEndian);

            int bigramSegmentSize = 1 << logBigramSegmentSize;
            int trigramSegTableSize = ((numberBigrams + 1) / bigramSegmentSize) + 1;
            trigramSegmentTable = readIntTable(stream, bigEndian,
                    trigramSegTableSize);
        }

        // read word string names
        int wordsStringLength = readInt(stream, bigEndian);
        if (wordsStringLength <= 0) {
            throw new Error("Bad word string size: " + wordsStringLength);
        }

        // read the string of all words
        this.words = readWords(stream, wordsStringLength, numberUnigrams);

        if (startWordID > -1) {
            UnigramProbability unigram = unigrams[startWordID];
            unigram.setLogProbability(MIN_PROBABILITY);
        }
        if (endWordID > -1) {
            UnigramProbability unigram = unigrams[endWordID];
            unigram.setLogBackoff(MIN_PROBABILITY);
        }

        applyUnigramWeight();
        if (applyLanguageWeightAndWip) {
            applyLanguageWeight(bigramProbTable, languageWeight);
            applyWip(bigramProbTable, wip);
            applyLanguageWeight(trigramProbTable, languageWeight);
            applyWip(trigramProbTable, wip);
            applyLanguageWeight(trigramBackoffTable, languageWeight);
        }

        stream.close();
        file = new RandomAccessFile(location, "r");
    }


    /**
     * Reads the LM file header
     *
     * @param stream the data stream of the LM file
     */
    private void readHeader(DataInputStream stream) throws IOException {
        int headerLength = readInt(stream, bigEndian);

        if (headerLength != (DARPA_LM_HEADER.length() + 1)) { // not big-endian
            headerLength = Utilities.swapInteger(headerLength);
            if (headerLength == (DARPA_LM_HEADER.length() + 1)) {
                bigEndian = false;
                // System.out.println("Little-endian");
            } else {
                throw new Error("Bad binary LM file magic number: "
                        + headerLength + ", not an LM dumpfile?");
            }
        } else {
            // System.out.println("Big-endian");
        }

        // read and verify standard header string
        String header = readString(stream, headerLength - 1);
        readByte(stream); // read the '\0'

        if (!header.equals(DARPA_LM_HEADER)) {
            throw new Error("Bad binary LM file header: " + header);
        }

        // read LM filename string size and string
        int fileNameLength = readInt(stream, bigEndian);
        bytesRead += stream.skipBytes(fileNameLength);

        numberUnigrams = 0;
        logBigramSegmentSize = LOG2_BIGRAM_SEGMENT_SIZE_DEFAULT;

        // read version number, if present. it must be <= 0.

        int version = readInt(stream, bigEndian);
        // System.out.println("Version: " + version);

        if (version <= 0) { // yes, its the version number
            readInt(stream, bigEndian); // read and skip timestamp

            // read and skip format description
            int formatLength;
            for (; ;) {
                if ((formatLength = readInt(stream, bigEndian)) == 0) {
                    break;
                }
                bytesRead += stream.skipBytes(formatLength);
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

        if (numberUnigrams <= 0) {
            throw new Error("Bad number of unigrams: " + numberUnigrams
                    + ", must be > 0.");
        } else {
            maxNGram = 1;
        }

        if ((numberBigrams = readInt(stream, bigEndian)) < 0) {
            throw new Error("Bad number of bigrams: " + numberBigrams);
        } else {
            maxNGram = 2;
        }

        if ((numberTrigrams = readInt(stream, bigEndian)) < 0) {
            throw new Error("Bad number of trigrams: " + numberTrigrams);
        } else {
            maxNGram = 3;
        }
    }


    /**
     * Skips the bigrams and trigrams of the LM.
     *
     * @param stream the source of data
     */
    private void skipBigramsTrigrams(DataInputStream stream) throws IOException {
        // skip all the bigram entries, the +1 is the sentinel at the end
        if (numberBigrams > 0) {
            bigramOffset = bytesRead;
            int bytesToSkip = (numberBigrams + 1)
                    * LargeTrigramModel.BYTES_PER_BIGRAM;
            stream.skipBytes(bytesToSkip);
            bytesRead += bytesToSkip;
        }

        // skip all the trigram entries
        if (numberTrigrams > 0) {
            trigramOffset = bytesRead;
            int bytesToSkip = numberTrigrams
                    * LargeTrigramModel.BYTES_PER_TRIGRAM;
            stream.skipBytes(bytesToSkip);
            bytesRead += bytesToSkip;
        }
    }


    /** Apply the unigram weight to the set of unigrams */
    private void applyUnigramWeight() {
        float logUnigramWeight = logMath.linearToLog(unigramWeight);
        float logNotUnigramWeight = logMath.linearToLog(1.0f - unigramWeight);
        float logUniform = logMath.linearToLog(1.0f / (numberUnigrams));

        float logWip = logMath.linearToLog(wip);

        float p2 = logUniform + logNotUnigramWeight;

        for (int i = 0; i < numberUnigrams; i++) {
            UnigramProbability unigram = unigrams[i];

            float p1 = unigram.getLogProbability();

            if (i != startWordID) {
                p1 += logUnigramWeight;
                p1 = logMath.addAsLinear(p1, p2);
            }

            if (applyLanguageWeightAndWip) {
                p1 = p1 * languageWeight + logWip;
                unigram.setLogBackoff(unigram.getLogBackoff() * languageWeight);
            }

            unigram.setLogProbability(p1);
        }
    }


    /** Apply the language weight to the given array of probabilities. */
    private void applyLanguageWeight(float[] logProbabilities,
                                     float languageWeight) {
        for (int i = 0; i < logProbabilities.length; i++) {
            logProbabilities[i] = logProbabilities[i] * languageWeight;
        }
    }


    /** Apply the WIP to the given array of probabilities. */
    private void applyWip(float[] logProbabilities, double wip) {
        float logWip = logMath.linearToLog(wip);
        for (int i = 0; i < logProbabilities.length; i++) {
            logProbabilities[i] = logProbabilities[i] + logWip;
        }
    }


    /**
     * Reads the probability table from the given DataInputStream.
     *
     * @param stream    the DataInputStream from which to read the table
     * @param bigEndian true if the given stream is bigEndian, false otherwise
     */
    private float[] readFloatTable(DataInputStream stream, boolean bigEndian)
            throws IOException {

        int numProbs = readInt(stream, bigEndian);
        if (numProbs <= 0 || numProbs > MAX_PROB_TABLE_SIZE) {
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
     * @param stream    the DataInputStream from which to read the table
     * @param bigEndian true if the given stream is bigEndian, false otherwise
     * @param tableSize the size of the trigram segment table
     * @return the trigram segment table, which is an array of integers
     */
    private int[] readIntTable(DataInputStream stream, boolean bigEndian,
                               int tableSize) throws IOException {
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
     * @param stream         the DataInputStream to read from
     * @param numberUnigrams the number of unigrams to read
     * @param bigEndian      true if the DataInputStream is big-endian, false otherwise
     * @return an array of UnigramProbability index by the unigram ID
     */
    private UnigramProbability[] readUnigrams(DataInputStream stream,
                                              int numberUnigrams, boolean bigEndian) throws IOException {

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

            unigrams[i] = new UnigramProbability(unigramID, logProbability,
                    logBackoff, firstBigramEntry);
        }

        return unigrams;
    }


    /**
     * Reads a byte from the given DataInputStream.
     *
     * @param stream the DataInputStream to read from
     * @return the byte read
     */
    private final byte readByte(DataInputStream stream) throws IOException {
        bytesRead++;
        return stream.readByte();
    }


    /**
     * Reads an integer from the given DataInputStream.
     *
     * @param stream    the DataInputStream to read from
     * @param bigEndian true if the DataInputStream is in bigEndian, false otherwise
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
     * @param stream    the DataInputStream to read from
     * @param bigEndian true if the DataInputStream is in bigEndian, false otherwise
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
     * Reads a string of the given length from the given DataInputStream. It is assumed that the DataInputStream
     * contains 8-bit chars.
     *
     * @param stream the DataInputStream to read from
     * @param length the number of characters in the returned string
     * @return a string of the given length from the given DataInputStream
     */
    private final String readString(DataInputStream stream, int length)
            throws IOException {
        StringBuffer buffer = new StringBuffer();
        byte[] bytes = new byte[length];
        bytesRead += stream.read(bytes);

        for (int i = 0; i < length; i++) {
            buffer.append((char) bytes[i]);
        }
        return buffer.toString();
    }


    /**
     * Reads a series of consecutive Strings from the given stream.
     *
     * @param stream         the DataInputStream to read from
     * @param length         the total length in bytes of all the Strings
     * @param numberUnigrams the number of String to read
     * @return an array of the Strings read
     */
    private final String[] readWords(DataInputStream stream, int length,
                                     int numberUnigrams) throws IOException {
        String[] words = new String[numberUnigrams];
        byte[] bytes = new byte[length];
        bytesRead += stream.read(bytes);

        int s = 0;
        int wordStart = 0;
        for (int i = 0; i < length; i++) {
            char c = (char) (bytes[i] & 0xFF);
            bytesRead++;
            if (c == '\0') {
                // if its the end of a string, add it to the 'words' array
                words[s] = new String(bytes, wordStart, i - wordStart).toLowerCase();
                wordStart = i + 1;          
                if (words[s].equals(Dictionary.SENTENCE_START_SPELLING)) {
                    startWordID = s;
                } else if (words[s].equals(Dictionary.SENTENCE_END_SPELLING)) {
                    endWordID = s;
                }
                s++;
            }
        }
        assert (s == numberUnigrams);
        return words;
    }

}
