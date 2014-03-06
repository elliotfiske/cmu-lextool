package edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi;

import edu.cmu.sphinx.linguist.acoustic.Unit;


/**
 * Decision tree.
 */
public interface EventMap {

    /**
     * Maps speech unit to probability distribution function.
     *
     * @param pdfClass requested state
     * @param unit     unit of speech
     *
     * @return identifier of probability distribution function
     */
    public int map(int pdfClass, Unit unit);
}
