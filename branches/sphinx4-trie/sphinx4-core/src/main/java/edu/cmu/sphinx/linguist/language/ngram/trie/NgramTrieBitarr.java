package edu.cmu.sphinx.linguist.language.ngram.trie;

public class NgramTrieBitarr {

    private byte[] mem;
    
    public NgramTrieBitarr(int memLen) {
        mem = new byte[memLen];
    }

    public byte[] getArr() {
        return mem;
    }

    public int readInt(int memPtr, int bitOffset, int mask) {
        int idx = memPtr + (bitOffset >> 3);
        int value = mem[idx++] & 0xFF;
        value |= (mem[idx++] << 8) & 0xFFFF;
        value |= (mem[idx++] << 16) & 0xFFFFFF;
        value |= (mem[idx++] << 24) & 0xFFFFFFFF;
        value >>= (bitOffset & 7);
        value &= mask;
        return value;
    }

    public float readNegativeFloat(int memPtr, int bitOffset) {
        //cap
        return 0.0f;
    }

    public float readFloat(int memPtr, int bitOffset) {
        //cap
        return 0.0f;
    }

}
