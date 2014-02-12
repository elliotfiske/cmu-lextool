package edu.cmu.sphinx.linguist.acoustic.tiedstate;

import java.util.*;

import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.linguist.acoustic.*;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi.EventMap;

/**
 * {@link HMMManager} extension to load HMMs from Kaldi model.
 *
 * Initially empty this class creates HMMs on request.
 */
public class LazyHmmManager extends HMMManager {

    private final EventMap eventMap;
    private final Pool<Senone> senonePool;
    private final Map<String, Integer> symbolTable;

    public LazyHmmManager(EventMap eventMap,
                          Pool<Senone> senonePool,
                          Map<String, Integer> symbolTable)
    {
        this.eventMap = eventMap;
        this.senonePool = senonePool;
        this.symbolTable = symbolTable;
    }

    @Override
    public HMM get(HMMPosition position, Unit unit) {
        HMM hmm = super.get(position, unit);
        if (null != hmm) return hmm;

        int[] unitIds = new int[3];
        unitIds[1] = symbolTable.get(unit.getName());

        if (unit.isContextDependent()) {
            LeftRightContext context = (LeftRightContext) unit.getContext();
            Unit left = context.getLeftContext()[0];
            Unit right = context.getRightContext()[0];
            unitIds[0] = symbolTable.get(left.getName());
            unitIds[2] = symbolTable.get(right.getName());
        } else {
            unitIds[0] = symbolTable.get("SIL");
            unitIds[2] = symbolTable.get("SIL");
        }

        Senone[] senones = new Senone[3];
        senones[0] = senonePool.get(eventMap.map(0, unitIds));
        senones[1] = senonePool.get(eventMap.map(1, unitIds));
        senones[2] = senonePool.get(eventMap.map(2, unitIds));
        SenoneSequence ss = new SenoneSequence(senones);

        float[][] tmat = new float[4][4];
        Arrays.fill(tmat[3], LogMath.LOG_ZERO);
        LogMath logMath = LogMath.getInstance();

        for (int i = 0; i < tmat.length - 1; ++i) {
            Arrays.fill(tmat[i], LogMath.LOG_ZERO);
            tmat[i][i] = logMath.linearToLog(0.25);
            tmat[i][i + 1] = logMath.linearToLog(0.75);
        }

        System.err.format("Loaded senone for unit %s\n", unit);
        hmm = new SenoneHMM(unit, ss, tmat, position);
        put(hmm);

        return hmm;
    }
}
