package edu.cmu.sphinx.tools.dictator.models;

import edu.cmu.sphinx.linguist.acoustic.*;
import edu.cmu.sphinx.linguist.acoustic.tiedstate.*;
import edu.cmu.sphinx.util.Timer;
import edu.cmu.sphinx.util.props.*;

import java.io.IOException;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Copyright 1999-2006 Carnegie Mellon University.
 * Portions Copyright 2002 Sun Microsystems, Inc.
 * All Rights Reserved.  Use is subject to license terms.
 * <p/>
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 * <p/>
 * User: Peter Wolf
 * Date: May 14, 2006
 * Time: 10:00:47 AM
 */
public class Model implements AcousticModel, Configurable {

    /**
     * The property that defines the component used to load the acoustic models
     */
    public final static String PROP_LOADER = "loader";

    /**
     * The property that defines the unit manager
     */
    public final static String PROP_UNIT_MANAGER = "unitManager";

    /**
     * Controls whether we generate composites or CI units when no
     * context is given during a lookup.
     */
    public final static String PROP_USE_COMPOSITES = "useComposites";

    /**
     * The default value of PROP_USE_COMPOSITES.
     */
    public final static boolean PROP_USE_COMPOSITES_DEFAULT = true;


    /**
     * Model load timer
     */
    protected final static String TIMER_LOAD = "AM_Load";


    // -----------------------------
    // Configured variables
    // -----------------------------
    protected String name;
    private Logger logger;
    protected Loader loader;
    protected UnitManager unitManager;
    private boolean useComposites = false;
    private Properties properties;

    // ----------------------------
    // internal variables
    // -----------------------------
    transient protected Timer loadTimer;
    transient private Map<String,SenoneSequence> compositeSenoneSequenceCache = new HashMap<String, SenoneSequence>();
    private boolean allocated = false;


    /* (non-Javadoc)
     * @see edu.cmu.sphinx.util.props.Configurable#register(java.lang.String, edu.cmu.sphinx.util.props.Registry)
     */
    public void register(String name, Registry registry)
            throws PropertyException {
        this.name = name;
        registry.register(PROP_LOADER, PropertyType.COMPONENT);
        registry.register(PROP_UNIT_MANAGER, PropertyType.COMPONENT);
        registry.register(PROP_USE_COMPOSITES, PropertyType.BOOLEAN);
    }

    /* (non-Javadoc)
     * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
     */
    public void newProperties(PropertySheet ps) throws PropertyException {
        loader = (Loader) ps.getComponent(PROP_LOADER, Loader.class);
        unitManager = (UnitManager) ps.getComponent(PROP_UNIT_MANAGER,
                UnitManager.class);
        useComposites =
                ps.getBoolean(PROP_USE_COMPOSITES, PROP_USE_COMPOSITES_DEFAULT);
        logger = ps.getLogger();
    }

    /**
     * initialize this acoustic models with the given name and context.
     *
     * @throws java.io.IOException if the models could not be loaded
     */
    public void allocate() throws IOException {
        if (!allocated) {
            this.loadTimer = Timer.getTimer(TIMER_LOAD);
            loadTimer.start();
            loader.load();
            loadTimer.stop();
            logInfo();
            allocated = true;
        }
    }

    /* (non-Javadoc)
    * @see edu.cmu.sphinx.linguist.acoustic.AcousticModel#deallocate()
    */
    public void deallocate() {
    }

    /**
     * Returns the name of this AcousticModel, or null if it has no name.
     *
     * @return the name of this AcousticModel, or null if it has no name
     */
    public String getName() {
        return name;
    }


    /**
     * Gets a composite HMM for the given unit and context
     *
     * @param unit     the unit for the hmm
     * @param position the position of the unit within the word
     * @return a composite HMM
     */
    private HMM getCompositeHMM(Unit unit, HMMPosition position) {


        if (true) { // use a true composite
            Unit ciUnit = unitManager.getUnit(unit.getName(),
                    unit.isFiller(), Context.EMPTY_CONTEXT);

            SenoneSequence compositeSequence =
                    getCompositeSenoneSequence(unit, position);

            SenoneHMM contextIndependentHMM = (SenoneHMM)
                    lookupNearestHMM(ciUnit,
                            HMMPosition.UNDEFINED, true);
            float[][] tmat = contextIndependentHMM.getTransitionMatrix();
            return new SenoneHMM(unit, compositeSequence, tmat, position);
        } else { // BUG: just a test. use CI units instead of composites
            Unit ciUnit = lookupUnit(unit.getName());

            assert unit.isContextDependent();
            if (ciUnit == null) {
                logger.severe("Can't find HMM for " + unit.getName());
            }
            assert ciUnit != null;
            assert !ciUnit.isContextDependent();

            HMMManager mgr = loader.getHMMManager();
            HMM hmm = mgr.get(HMMPosition.UNDEFINED, ciUnit);
            return hmm;
        }
    }

    /**
     * Given a unit, returns the HMM that best matches the given unit.
     * If exactMatch is false and an exact match is not found,
     * then different word positions
     * are used. If any of the contexts are non-silence filler units.
     * a silence filler unit is tried instead.
     *
     * @param unit       the unit of interest
     * @param position   the position of the unit of interest
     * @param exactMatch if true, only an exact match is
     *                   acceptable.
     * @return the HMM that best matches, or null if no match
     *         could be found.
     */
    public HMM lookupNearestHMM(Unit unit, HMMPosition position,
                                boolean exactMatch) {

        if (exactMatch) {
            return lookupHMM(unit, position);
        } else {
            HMMManager mgr = loader.getHMMManager();
            HMM hmm = mgr.get(position, unit);

            if (hmm != null) {
                return hmm;
            }
            // no match, try a composite

            if (useComposites && hmm == null) {
                if (isComposite(unit)) {

                    hmm = getCompositeHMM(unit, position);
                    if (hmm != null) {
                        mgr.put(hmm);
                    }
                }
            }
            // no match, try at other positions
            if (hmm == null) {
                hmm = getHMMAtAnyPosition(unit);
            }
            // still no match, try different filler
            if (hmm == null) {
                hmm = getHMMInSilenceContext(unit, position);
            }

            // still no match, backoff to base phone
            if (hmm == null) {
                Unit ciUnit = lookupUnit(unit.getName());

                assert unit.isContextDependent();
                if (ciUnit == null) {
                    logger.severe("Can't find HMM for " + unit.getName());
                }
                assert ciUnit != null;
                assert !ciUnit.isContextDependent();

                hmm = mgr.get(HMMPosition.UNDEFINED, ciUnit);
            }

            assert hmm != null;

            // System.out.println("PROX match for "
            // 	+ unit + " at " + position + ":" + hmm);

            return hmm;
        }
    }

    /**
     * Determines if a unit is a composite unit
     *
     * @param unit the unit to test
     * @return true if the unit is missing a right context
     */
    private boolean isComposite(Unit unit) {

        if (unit.isFiller()) {
            return false;
        }

        Context context = unit.getContext();
        if (context instanceof LeftRightContext) {
            LeftRightContext lrContext = (LeftRightContext) context;
            if (lrContext.getRightContext() == null) {
                return true;
            }
            if (lrContext.getLeftContext() == null) {
                return true;
            }
        }
        return false;
    }

    /**
     * Looks up the context independent unit given
     * the name
     *
     * @param name the name of the unit
     * @return the unit or null if the unit was not found
     */
    private Unit lookupUnit(String name) {
        return (Unit) loader.getContextIndependentUnits().get(name);
    }

    /**
     * Returns an iterator that can be used to iterate through all
     * the HMMs of the acoustic models
     *
     * @return an iterator that can be used to iterate through all
     *         HMMs in the models. The iterator returns objects of type
     *         <code>HMM</code>.
     */
    public Iterator getHMMIterator() {
        return loader.getHMMManager().getIterator();
    }


    /**
     * Returns an iterator that can be used to iterate through all
     * the CI units in the acoustic models
     *
     * @return an iterator that can be used to iterate through all
     *         CI units. The iterator returns objects of type
     *         <code>Unit</code>
     */
    public Iterator getContextIndependentUnitIterator() {
        return loader.getContextIndependentUnits().values().iterator();
    }

    /**
     * Get a composite senone sequence given the unit
     * The unit should have a LeftRightContext, where one or two of
     * 'left' or 'right' may be null to indicate that the match
     * should succeed on any context.
     *
     * @param unit the unit
     */
    public SenoneSequence getCompositeSenoneSequence(Unit unit,
                                                     HMMPosition position) {
        Context context = unit.getContext();
        SenoneSequence compositeSenoneSequence = null;
        compositeSenoneSequence = compositeSenoneSequenceCache.get(unit.toString());

        if (logger.isLoggable(Level.FINE)) {
            logger.fine("getCompositeSenoneSequence: " + unit.toString()
                    + ((compositeSenoneSequence != null) ? "Cached" : ""));
        }
        if (compositeSenoneSequence != null) {
            return compositeSenoneSequence;
        }

        // Iterate through all HMMs looking for
        // a) An hmm with a unit that has the proper base
        // b) matches the non-null context

        List<SenoneSequence> senoneSequenceList = new ArrayList<SenoneSequence>();

        // collect all senone sequences that match the pattern
        for (Iterator i = getHMMIterator(); i.hasNext();) {
            SenoneHMM hmm = (SenoneHMM) i.next();
            if (hmm.getPosition() == position) {
                Unit hmmUnit = hmm.getUnit();
                if (hmmUnit.isPartialMatch(unit.getName(), context)) {
                    if (logger.isLoggable(Level.FINE)) {
                        logger.fine("collected: " + hmm.getUnit().toString());
                    }
                    senoneSequenceList.add(hmm.getSenoneSequence());
                }
            }
        }

        // couldn't find any matches, so at least include the CI unit
        if (senoneSequenceList.size() == 0) {
            Unit ciUnit = unitManager.getUnit(unit.getName(), unit.isFiller());
            SenoneHMM baseHMM = lookupHMM(ciUnit, HMMPosition.UNDEFINED);
            senoneSequenceList.add(baseHMM.getSenoneSequence());
        }

        // Add this point we have all of the senone sequences that
        // match the base/context pattern collected into the list.
        // Next we build a CompositeSenone consisting of all of the
        // senones in each position of the list.

        // First find the longest senone sequence

        int longestSequence = 0;
        for (int i = 0; i < senoneSequenceList.size(); i++) {
            SenoneSequence ss = senoneSequenceList.get(i);
            if (ss.getSenones().length > longestSequence) {
                longestSequence = ss.getSenones().length;
            }
        }

        // now collect all of the senones at each position into
        // arrays so we can create CompositeSenones from them
        // QUESTION: is is possible to have different size senone
        // sequences. For now lets assume the worst case.

        List<CompositeSenone> compositeSenones = new ArrayList<CompositeSenone>();
        float logWeight = 0.0f;
        for (int i = 0; i < longestSequence; i++) {
            Set<Senone> compositeSenoneSet = new HashSet<Senone>();
            for (int j = 0; j < senoneSequenceList.size(); j++) {
                SenoneSequence senoneSequence =
                        senoneSequenceList.get(j);
                if (i < senoneSequence.getSenones().length) {
                    Senone senone = senoneSequence.getSenones()[i];
                    compositeSenoneSet.add(senone);
                }
            }
            compositeSenones.add(CompositeSenone.create(
                    compositeSenoneSet, logWeight));
        }

        compositeSenoneSequence = SenoneSequence.create(compositeSenones);
        compositeSenoneSequenceCache.put(unit.toString(),
                compositeSenoneSequence);

        if (logger.isLoggable(Level.FINE)) {
            logger.fine(unit.toString() + " consists of " +
                    compositeSenones.size() + " composite senones");
            if (logger.isLoggable(Level.FINEST)) {
                compositeSenoneSequence.dump("am");
            }
        }
        return compositeSenoneSequence;
    }


    /**
     * Returns the size of the left context for context dependent
     * units
     *
     * @return the left context size
     */
    public int getLeftContextSize() {
        return loader.getLeftContextSize();
    }

    /**
     * Returns the size of the right context for context dependent
     * units
     *
     * @return the left context size
     */
    public int getRightContextSize() {
        return loader.getRightContextSize();
    }


    /**
     * Given a unit, returns the HMM that exactly matches the given
     * unit.
     *
     * @param unit     the unit of interest
     * @param position the position of the unit of interest
     * @return the HMM that exactly matches, or null if no match
     *         could be found.
     */
    private SenoneHMM lookupHMM(Unit unit, HMMPosition position) {
        return (SenoneHMM) loader.getHMMManager().get(position, unit);
    }


    /**
     * Creates a string useful for tagging a composite senone sequence
     *
     * @param base    the base unit
     * @param context the context
     * @return the tag associated with the composite senone sequence
     */
    private String makeTag(Unit base, Context context) {
        StringBuffer sb = new StringBuffer();
        sb.append("(");
        sb.append(base.getName());
        sb.append("-");
        sb.append(context.toString());
        sb.append(")");
        return sb.toString();
    }


    /**
     * Dumps information about this models to the logger
     */
    protected void logInfo() {
        if (loader != null) {
            loader.logInfo();
        }
        logger.info("CompositeSenoneSequences: " +
                compositeSenoneSequenceCache.size());
    }


    /**
     * Searches an hmm at any position
     *
     * @param unit the unit to search for
     * @return hmm the hmm or null if it was not found
     */
    private SenoneHMM getHMMAtAnyPosition(Unit unit) {
        SenoneHMM hmm = null;
        HMMManager mgr = loader.getHMMManager();
        for (Iterator i = HMMPosition.iterator();
             hmm == null && i.hasNext();) {
            HMMPosition pos = (HMMPosition) i.next();
            hmm = (SenoneHMM) mgr.get(pos, unit);
        }
        return hmm;
    }

    /**
     * Given a unit, search for the HMM associated with this unit by
     * replacing all non-silence filler contexts with the silence
     * filler context
     *
     * @param unit the unit of interest
     * @return the associated hmm or null
     */
    private SenoneHMM getHMMInSilenceContext(Unit unit, HMMPosition position) {
        SenoneHMM hmm = null;
        HMMManager mgr = loader.getHMMManager();
        Context context = unit.getContext();

        if (context instanceof LeftRightContext) {
            LeftRightContext lrContext = (LeftRightContext) context;

            Unit[] lc = lrContext.getLeftContext();
            Unit[] rc = lrContext.getRightContext();

            Unit[] nlc;
            Unit[] nrc;

            if (hasNonSilenceFiller(lc)) {
                nlc = replaceNonSilenceFillerWithSilence(lc);
            } else {
                nlc = lc;
            }

            if (hasNonSilenceFiller(rc)) {
                nrc = replaceNonSilenceFillerWithSilence(rc);
            } else {
                nrc = rc;
            }

            if (nlc != lc || nrc != rc) {
                Context newContext = LeftRightContext.get(nlc, nrc);
                Unit newUnit = unitManager.getUnit(unit.getName(),
                        unit.isFiller(), newContext);
                hmm = (SenoneHMM) mgr.get(position, newUnit);
                if (hmm == null) {
                    hmm = getHMMAtAnyPosition(newUnit);
                }
            }
        }
        return hmm;
    }


    /**
     * Some debugging code that looks for illformed contexts
     *
     * @param msg the message associated with the check
     * @param c   the context to check
     */
    private void checkNull(String msg, Unit[] c) {
        for (int i = 0; i < c.length; i++) {
            if (c[i] == null) {
                System.out.println("null at index " + i + " of " + msg);
            }
        }
    }


    /**
     * Returns true if the array of units contains
     * a non-silence filler
     *
     * @param units the units to check
     * @return true if the array contains a filler that is not the
     *         silence filler
     */
    private boolean hasNonSilenceFiller(Unit[] units) {
        if (units == null) {
            return false;
        }

        for (int i = 0; i < units.length; i++) {
            if (units[i].isFiller() &&
                    !units[i].equals(UnitManager.SILENCE)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns a unit array with all non-silence filler units replaced
     * with the silence filler
     * a non-silence filler
     *
     * @param context the context to check
     * @return true if the array contains a filler that is not the
     *         silence filler
     */
    private Unit[] replaceNonSilenceFillerWithSilence(Unit[] context) {
        Unit[] replacementContext = new Unit[context.length];
        for (int i = 0; i < context.length; i++) {
            if (context[i].isFiller() &&
                    !context[i].equals(UnitManager.SILENCE)) {
                replacementContext[i] = UnitManager.SILENCE;
            } else {
                replacementContext[i] = context[i];
            }
        }
        return replacementContext;
    }


    /**
     * Returns the properties of this acoustic models.
     *
     * @return the properties of this acoustic models
     */
    public Properties getProperties() {
        if (properties == null) {
            properties = new Properties();
            try {
                properties.load
                        (getClass().getResource("models.props").openStream());
            } catch (IOException ioe) {
                ioe.printStackTrace();
            }
        }
        return properties;
    }
}
