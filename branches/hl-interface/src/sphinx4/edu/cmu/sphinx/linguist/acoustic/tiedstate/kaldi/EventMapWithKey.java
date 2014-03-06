package edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi;

import edu.cmu.sphinx.linguist.acoustic.Context;
import edu.cmu.sphinx.linguist.acoustic.LeftRightContext;
import edu.cmu.sphinx.linguist.acoustic.Unit;


public abstract class EventMapWithKey implements EventMap {

    protected final int key;

    protected EventMapWithKey(int key) {
        this.key = key;
    }

    protected int getKeyValue(int pdfClass, Unit unit) {
        if (-1 == key)
            return pdfClass;

        if (1 == key)
            return unit.getBaseID();

        if (0 == key) {
            LeftRightContext context = (LeftRightContext) unit.getContext();
            return context.getLeftContext()[0].getBaseID();
        }

        if (2 == key) {
            LeftRightContext context = (LeftRightContext) unit.getContext();
            return context.getRightContext()[0].getBaseID();
        }

        throw new IllegalStateException("invalid key " + key);
    }
}
