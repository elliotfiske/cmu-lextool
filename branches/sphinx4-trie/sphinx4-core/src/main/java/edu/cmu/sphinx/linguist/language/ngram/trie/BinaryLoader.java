package edu.cmu.sphinx.linguist.language.ngram.trie;

import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;

import edu.cmu.sphinx.linguist.language.ngram.trie.NgramTrie.TrieUnigram;
import edu.cmu.sphinx.util.Utilities;

public class BinaryLoader {

    private static final String TRIE_HEADER = "Trie Language Model";

    DataInputStream inStream;

    public BinaryLoader(File location) throws IOException {
        inStream = new DataInputStream(new FileInputStream(location));
    }

    public void verifyHeader() throws IOException {
        String readHeader = readString(inStream, TRIE_HEADER.length());
        if (!readHeader.equals(TRIE_HEADER)) {
            throw new Error("Bad binary LM file header: " + readHeader);
        }
    }

    public int[] readCounts() throws IOException {
        int order = readOrder();
        int[] counts = new int[order];
        for (int i = 0; i < counts.length; i++) {
            counts[i] = Utilities.readLittleEndianInt(inStream);
        }
        return counts;
    }

    public NgramTrieQuant readQuant(int order) throws IOException {
        int quantTypeInt = Utilities.readLittleEndianInt(inStream);
        if (quantTypeInt < 0 || quantTypeInt >= NgramTrieQuant.QuantType.values().length)
            throw new Error("Unknown quantatization type: " + quantTypeInt);
        NgramTrieQuant.QuantType quantType = NgramTrieQuant.QuantType.values()[quantTypeInt];
        NgramTrieQuant quant = new NgramTrieQuant(order, quantType);
        //reading tables
        for (int i = 2; i <= order; i++) {
            quant.setTable(readFloatArr(quant.getProbTableLen()), i, true);
            if (i < order)
                quant.setTable(readFloatArr(quant.getBackoffTableLen()), i, false);
        }
        return quant;
    }

    public TrieUnigram[] readUnigrams(int count) throws IOException {
        TrieUnigram[] unigrams = new TrieUnigram[count + 1];
        for (int i = 0; i < count + 1; i++) {
            TrieUnigram unigram = new TrieUnigram();
            unigram.prob = Utilities.readLittleEndianFloat(inStream);
            unigram.backoff = Utilities.readLittleEndianFloat(inStream);
            unigram.next = Utilities.readLittleEndianInt(inStream);
        }
        return unigrams;
    }

    private int readOrder() throws IOException {
        return (int)inStream.readByte();
    }

    private float[] readFloatArr(int len) throws IOException {
        float[] arr = new float[len];
        for (int i = 0; i < len; i++)
            arr[i] = Utilities.readLittleEndianFloat(inStream);
        return arr;
    }

    /**
     * Reads a string of the given length from the given DataInputStream. It is assumed that the DataInputStream
     * contains 8-bit chars.
     *
     * @param stream the DataInputStream to read from
     * @param length the number of characters in the returned string
     * @return a string of the given length from the given DataInputStream
     * @throws java.io.IOException
     */
    private String readString(DataInputStream stream, int length)
            throws IOException {
        StringBuilder builder = new StringBuilder();
        byte[] bytes = new byte[length];
        stream.read(bytes);
        for (int i = 0; i < length; i++) {
            builder.append((char) bytes[i]);
        }
        return builder.toString();
    }

}
