package edu.cmu.sphinx.linguist.language.ngram.trie;

import edu.cmu.sphinx.linguist.language.ngram.trie.NgramTrieModel.TrieRange;

//import edu.cmu.sphinx.linguist.language.ngram.trie.NgramTrie.Range;

public class NgramTrie {

    private MiddleNgram[] middles;
    private LongestNgram longest;
    private NgramTrieBitarr bitArr;
    private int ordersNum;
    private int quantProbBoLen;
    private int quantProbLen;

    public NgramTrie(int[] counts, int quantProbBoLen, int quantProbLen) {
        int memLen = 0;
        int[] ngramMemSize = new int[counts.length - 1];
        for (int i = 1; i <= counts.length - 1; i++) {
            int entryLen = requiredBits(counts[0]);
            if (i == counts.length - 1) {
                //longest ngram
                entryLen += quantProbLen;
            } else {
                //middle ngram
                entryLen += requiredBits(counts[i + 1]);
                entryLen += quantProbBoLen;
            }
            // Extra entry for next pointer at the end.  
            // +7 then / 8 to round up bits and convert to bytes
            // +8 (or +sizeof(uint64))so that reading bit array doesn't exceed bounds 
            // Note that this waste is O(order), not O(number of ngrams).
            int tmpLen = ((1 + counts[i]) * entryLen + 7) / 8 + 8; 
            ngramMemSize[i - 1] = tmpLen;
            memLen += tmpLen;
        }
        bitArr = new NgramTrieBitarr(memLen);
        this.quantProbLen = quantProbLen;
        this.quantProbBoLen = quantProbBoLen;
        middles = new MiddleNgram[counts.length - 2];
        int[] startPtrs = new int[counts.length - 2];
        int startPtr = 0;
        for (int i = 0; i < counts.length - 2; i++) {
            startPtrs[i] = startPtr;
            startPtr += ngramMemSize[i];
        }
        // Crazy backwards thing so we initialize using pointers to ones that have already been initialized
        for (int i = counts.length - 1; i >= 2; --i) {
            middles[i - 2] = new MiddleNgram(startPtrs[i - 2], quantProbBoLen, counts[i-1], counts[0], counts[i]);
        }
        longest = new LongestNgram(startPtr, quantProbLen, counts[0]);
        ordersNum = middles.length + 1;
    }

    public byte[] getMem() {
        return bitArr.getArr();
    }

    private int findNgram(Ngram ngramSet, int wordId, TrieRange range) {
        int ptr;
        range.begin--;
        if ((ptr = uniformFind(ngramSet, range, wordId)) < 0) {
            range.setInvalid();
            return -1;
        }
        //read next order ngrams for future searches
        if (ngramSet instanceof MiddleNgram)
            ((MiddleNgram)ngramSet).readNextRange(ptr, range);
        return ptr;
    }

    public float readNgramBackoff(int wordId, int orderMinusTwo, TrieRange range, NgramTrieQuant quant) {
        int ptr;
        Ngram ngram = getNgram(orderMinusTwo);
        if ((ptr = findNgram(ngram, wordId, range)) < 0)
            return 0.0f;
        return quant.readBackoff(bitArr, ngram.memPtr, ngram.getNgramWeightsOffset(ptr), orderMinusTwo);
    }

    public float readNgramProb(int wordId, int orderMinusTwo, TrieRange range, NgramTrieQuant quant) {
        int ptr;
        Ngram ngram = getNgram(orderMinusTwo);
        if ((ptr = findNgram(ngram, wordId, range)) < 0)
            return 0.0f;
        return quant.readProb(bitArr, ngram.memPtr, ngram.getNgramWeightsOffset(ptr), orderMinusTwo);
    }

    private int calculatePivot(int offset, int range, int width) {
    	return (offset * width) / (range + 1);
    }

    private int uniformFind(Ngram ngram, TrieRange range, int wordId) {
    	TrieRange vocabRange = new TrieRange(0, ngram.maxVocab);
        while (range.width() > 1) {
            int pivot = range.begin + 1 + calculatePivot(wordId - vocabRange.begin, vocabRange.width(), range.width() - 1);
            int mid = ngram.readNgramWord(pivot);
            if (mid < wordId) {
                range.begin = pivot;
                vocabRange.begin = mid;
            } else if (mid > wordId){
                range.end = pivot;
                vocabRange.end = mid;
            } else {
                return pivot;
            }
        }
        return -1;
    }

    private Ngram getNgram(int orderMinusTwo) {
        if (orderMinusTwo == ordersNum - 1)
            return longest;
        return middles[orderMinusTwo];
    }

    private int requiredBits(int maxValue) {
        if (maxValue == 0) return 0;
        int res = 1;
        while ((maxValue >>= 1) != 0) res++;
        return res;
    }

    class BitMask {
        int bits;
        int mask;
        BitMask(int maxValue) {
            bits = requiredBits(maxValue);
            mask = (1 << bits) - 1;
        }
    }

    abstract class Ngram {
        int memPtr;
        int wordBits;
        int wordMask;
        int totalBits;
        int insertIdx;
        int maxVocab;
        Ngram(int memPtr, int maxVocab, int remainingBits) {
            this.maxVocab = maxVocab;
            this.memPtr = memPtr;
            wordBits = requiredBits(maxVocab);
            if (wordBits > 25)
                throw new Error("Sorry, word indices more than" + (1 << 25) + " are not implemented");
            totalBits = wordBits + remainingBits;
            wordMask = (1 << wordBits) - 1;
            insertIdx = 0;
        }

        int readNgramWord(int ngramIdx) {
            int offset = ngramIdx * totalBits;
            return bitArr.readInt(memPtr, offset, wordMask);
        }

        int getNgramWeightsOffset(int ngramIdx) {
            return ngramIdx * totalBits + wordBits;
        }

        abstract int getQuantBits();

    }

    class MiddleNgram extends Ngram {
        BitMask nextMask;
        int nextOrderMemPtr;
        MiddleNgram(int memPtr, int quantBits, int entries, int maxVocab, int maxNext) {
            super(memPtr, maxVocab, quantBits + requiredBits(maxNext));
            nextMask = new BitMask(maxNext);
            if (entries + 1 >= (1 << 25) || (maxNext >= (1 << 25)))
                throw new Error("Sorry, current implementation doesn't support more than " + (1 << 25) + " n-grams of particular order");
        }

        void readNextRange(int ngramIdx, TrieRange range) {
            int offset = ngramIdx * totalBits;
            offset += wordBits;
            offset += getQuantBits();
            range.begin = bitArr.readInt(memPtr, offset, nextMask.mask);
            offset += totalBits;
            range.end = bitArr.readInt(memPtr, offset, nextMask.mask);
        }

        @Override
        int getQuantBits() {
            return quantProbBoLen;
        }
    }

    class LongestNgram extends Ngram {
        LongestNgram(int memPtr, int quantBits, int maxVocab) {
            super(memPtr, maxVocab, quantBits);
        }

        @Override
        int getQuantBits() {
            return quantProbLen;
        }
    }

}
