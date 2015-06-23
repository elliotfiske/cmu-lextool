package edu.cmu.sphinx.linguist.language.ngram.trie;

public class NgramTrieQuant {

    public static enum QuantType {NO_QUANT, QUANT_16};

    private int probBits;
    private int backoffBits;

    private int probMask;
    private int backoffMask;
    private float[][] tables;

    public NgramTrieQuant(int order, QuantType quantType) {
        switch (quantType) {
        case NO_QUANT:
            return; //nothing to do here
        case QUANT_16:
            probBits = 16;
            backoffBits = 16;
            probMask = (1 << probBits) - 1;
            backoffMask = (1 << backoffBits) - 1;
            break;
        default:
            throw new Error("Unsupported quantation type: " + quantType);
        }
        tables = new float[(order - 1) * 2 - 1][];
    }
    
    public void setTable(float[] table, int order, boolean isProb) {
        int index = (order - 2) * 2;
        if (!isProb) index++;
        tables[index] = table;
    }

    public int getProbTableLen() {
        return 1 << probBits;
    }

    public int getBackoffTableLen() {
        return 1 << backoffBits;
    }
}
