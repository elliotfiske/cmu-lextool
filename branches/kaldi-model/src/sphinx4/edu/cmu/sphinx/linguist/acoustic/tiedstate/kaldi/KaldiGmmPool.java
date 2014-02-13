package edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi;

import edu.cmu.sphinx.linguist.acoustic.tiedstate.Pool;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.Senone;


public class KaldiGmmPool extends Pool<Senone> {

    public KaldiGmmPool(KaldiTextParser parser) {
        super("senones");
        parser.expectToken("<DIMENSION>");
        // Skip dimension value.
        parser.getInt();
        parser.expectToken("<NUMPDFS>");
        int npdf = parser.getInt();

        for (int i = 0; i < npdf; ++i)
            put(i, new DiagGmm(i, parser));
    }
}
