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

package edu.cmu.sphinx.decoder.linguist.lextree;


import edu.cmu.sphinx.knowledge.acoustic.HMM;
import edu.cmu.sphinx.knowledge.acoustic.HMMState;
import edu.cmu.sphinx.knowledge.acoustic.HMMStateArc;
import edu.cmu.sphinx.knowledge.acoustic.HMMPosition;
import edu.cmu.sphinx.knowledge.acoustic.Unit;
import edu.cmu.sphinx.knowledge.acoustic.AcousticModel;
import edu.cmu.sphinx.knowledge.acoustic.Context;
import edu.cmu.sphinx.knowledge.acoustic.LeftRightContext;
import edu.cmu.sphinx.util.Timer;

import java.util.Iterator;
import java.util.Map;
import java.util.HashMap;


/**
 * The HMMPool provides the ability to manage units via small integer
 * IDs.  Context Independent units and context dependent units can
 * be converted to an ID. IDs can be used to quickly retrieve a unit
 * or an hmm associated with the unit.  This class operates under the
 * constraint that context sizes are exactly one, which is generally
 * only valid for large vocabulary tasks.
 */
class HMMPool {
    private AcousticModel model;
    private int maxCIUnits = 0;
    private Unit[] unitTable;
    private Map unitMap;

    HMMPool(AcousticModel model) {
        this.model = model;

        if (model.getLeftContextSize() != 1) {
            throw new Error("LexTreeLinguist: Unsupported left context size");
        }

        if (model.getRightContextSize() != 1) {
            throw new Error("LexTreeLinguist: Unsupported right context size");
        }

        // count CI units:

        unitMap = new HashMap();
        int curID = 1;
        for (Iterator i = model.getContextIndependentUnitIterator();
                i.hasNext();) {
            Unit unit = (Unit) i.next();
            unitMap.put(unit.getName(), new Integer(curID++));
        }

        maxCIUnits = curID;

        unitTable = new Unit[maxCIUnits * maxCIUnits * maxCIUnits];

        for (Iterator i = model.getHMMIterator(); i.hasNext(); ) {
            HMM hmm = (HMM) i.next();
            Unit unit = hmm.getUnit();
            int id = getID(unit);
            unitTable[id] = unit;
            // System.out.println("Unit " + unit + " id " + id);
        }
    }


    /**
     * Gets the unit for the given id
     *
     * @param unitID the id for the unit
     *
     * @returns the unit associated with the ID
     */
    Unit getUnit(int unitID) {
        return unitTable[unitID];
    }


    /**
     * Given a unit id and a position, return the HMM
     * associated with the unit/position
     *
     * @param unitID the id of the unit
     * @param position the position within the word
     *
     * @return the hmm associated with the unit/position
     */
    HMM getHMM(int unitID, HMMPosition position) {
        Unit unit = getUnit(unitID);
        assert unit != null;
        return model.lookupNearestHMM(unit, position);
    }

    /**
     * given a unit return its ID
     *
     * @param unit the unit
     *
     * @return an ID
     */
    int getID(Unit unit) {
        if (unit.isContextDependent()) {
            LeftRightContext context = (LeftRightContext) unit.getContext();
            assert context.getLeftContext().length == 1;
            assert context.getRightContext().length == 1;
            return buildID(
                getSimpleUnitID(unit),
                getSimpleUnitID(context.getLeftContext()[0]),
                getSimpleUnitID(context.getRightContext()[0]));
        } else {
            return getSimpleUnitID(unit);
        }
    }

    /**
     * Returns a context independent ID
     *
     * @param unit the unit of interest
     *
     * @return the ID of the central unit (ignoring any context)
     */
    int getSimpleUnitID(Unit unit) {
        Integer id = (Integer) unitMap.get(unit.getName());
        if (id == null) {
            throw new Error("Can't find " + unit + " in unitMap");
        }
        return  id.intValue();
    }

    /**
     * Builds an id from the given unit and its left and right unit
     * ids
     *
     * @param unitID the id of the central unit
     * @param leftID the id of the left context unit
     * @param rightID the id of the right context unit
     *
     * @return the id for the context dependent unit
     */
    int buildID(int unitID, int leftID, int rightID) {
        return unitID * (maxCIUnits * maxCIUnits)
              + (leftID * maxCIUnits) 
              + rightID ;
    }

    /**
     * Given a unit id extract the left context unit id
     *
     * @param id the unit id
     *
     * @return the unit id of the left context (0 means no left
     * context)
     */
    int getLeftUnitID(int id) {
        return (id / maxCIUnits) % maxCIUnits;
    }

    /**
     * Given a unit id extract the right context unit id
     *
     * @param id the unit id
     *
     * @return the unit id of the right context (0 means no right
     * context)
     */
    int getRightUnitID(int id) {
        return id % maxCIUnits;
    }

    /**
     * Given a unit id extract the centeral unit id
     *
     * @param id the unit id
     *
     * @return the central unit id 
     */
    int getCentralUnitID(int id) {
        return id / (maxCIUnits * maxCIUnits);
    }

    /**
     * Dumps out info about this pool
     */
    void dumpInfo() {
        System.out.println("Max CI Units " + maxCIUnits);
        System.out.println("Unit table size " + unitTable.length);
        System.out.println("Unit map size " + unitMap.size());

        if (false) {
            for (int i = 0; i < unitTable.length; i++) {
                System.out.println("" + i + " " + unitTable[i]);
            }
        }
    }


    /**
     * A quick and dirty benchmark to get an idea how long
     * the HMM lookups will take.  This experiment shows
     * that on a 1GHZ sparc system, the lookup takes a little
     * less than 1uSec.  This is probably fast enough.
     */

    static HMMPosition pos[] = {
        HMMPosition.BEGIN, HMMPosition.END, HMMPosition.SINGLE,
        HMMPosition.INTERNAL};

   static int ids[] = { 9206, 9320, 9620, 9865, 14831, 15836 };

    void benchmark() {
        int nullCount = 0;
        System.out.println("benchmarking ...");
        Timer.start("hmmPoolBenchmark");

        for (int i = 0; i < 1000000; i++) {
            int id = ids[i % ids.length];
            HMMPosition position = pos[i % pos.length];
            HMM hmm = getHMM(id, position);
            if (hmm == null) {
                nullCount++;
            }
        }
        Timer.stop("hmmPoolBenchmark");
        System.out.println("null count " + nullCount);
    }
}

