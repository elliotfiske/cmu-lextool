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

package edu.cmu.sphinx.knowledge.acoustic;

import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.Timer;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Set;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.logging.Logger;
import java.util.logging.Level;
import java.util.Map;
import java.util.StringTokenizer;

/**
 * Represents the generic interface to the Acoustic 
 * Model for sphinx4
 */
public class AcousticModel {

    static Map contextMap = new HashMap();


    /**
     * Prefix for acoustic model SphinxProperties.
     */
    public final static String PROP_PREFIX
	= "edu.cmu.sphinx.knowledge.acoustic.";


    /**
     * The SphinxProperty name for the names of all the acoustic models
     */
    public final static String PROP_NAMES = PROP_PREFIX + "names";


    /**
     * The default value for PROP_NAMES.
     */
    public final static String PROP_NAMES_DEFAULT = null;


    /**
     * The format for the acoustic model data. Current supported
     * formats are:
     *
     *  sphinx3.ascii
     *  sphinx3.binary
     *  sphinx4.ascii
     *  sphinx4.binary
     */
    public final static String PROP_FORMAT = PROP_PREFIX + "format";


    /**
     * The default value of PROP_FORMAT.
     */
    public final static String PROP_FORMAT_DEFAULT = "sphinx3.binary";


    /**
     * The directory where the acoustic model data can be found.
     */
    public final static String PROP_LOCATION = PROP_PREFIX + "location";


    /**
     * The default value of PROP_LOCATION.
     */
    public final static String PROP_LOCATION_DEFAULT = ".";


    /**
     * The name of the model definition file (contains the HMM data)
     */
    public final static String PROP_MODEL = PROP_PREFIX + "definition_file";


    /**
     * The default value of PROP_MODEL_DEFAULT.
     */
    public final static String PROP_MODEL_DEFAULT = "model.mdef";


    /**
     * Subdirectory of PROP_LOCATION where the acoustic model can be found
     */
    public final static String PROP_DATA_LOCATION
	= PROP_PREFIX + "data_location";


    /**
     * The default value of PROP_DATA_LOCATION.
     */
    public final static String PROP_DATA_LOCATION_DEFAULT = "data";


    /**
     * The SphinxProperty for the name of the acoustic properties file.
     */
    public final static String PROP_PROPERTIES_FILE
	= PROP_PREFIX + "properties_file";


    /**
     * The default value of PROP_PROPERTIES_FILE.
     */
    public final static String PROP_PROPERTIES_FILE_DEFAULT = "am.props";


    /**
     * The SphinxProperty for the length of feature vectors.
     */
    public final static String PROP_VECTOR_LENGTH
	= PROP_PREFIX + "FeatureVectorLength";


    /**
     * The default value of PROP_VECTOR_LENGTH.
     */
    public final static int PROP_VECTOR_LENGTH_DEFAULT = 39;


    /**
     * The SphinxProperty specifying whether the transition matrices
     * of the acoustic model is in sparse form, i.e., omitting
     * the zeros of the non-transitioning states.
     */
    public final static String PROP_SPARSE_FORM = PROP_PREFIX + "sparseForm";


    /**
     * The default value of PROP_SPARSE_FORM.
     */
    public final static boolean PROP_SPARSE_FORM_DEFAULT = true;


    /**
     * The SphinxProperty specifying whether context-dependent units
     * should be used.
     */
    public final static String PROP_USE_CD_UNITS = PROP_PREFIX + "useCDUnits";


    /**
     * The default value of PROP_USE_CD_UNITS.
     */
    public final static boolean PROP_USE_CD_UNITS_DEFAULT = true;


    /**
     * Controls whether we generate composites or CI units when no
     * context is given during a lookup.
     */
    public final static String PROP_USE_COMPOSITES
	= PROP_PREFIX + "useComposites";


    /**
     * The default value of PROP_USE_COMPOSITES.
     */
    public final static boolean PROP_USE_COMPOSITES_DEFAULT = true;


    /**
     * Mixture component score floor.
     */
    public final static String PROP_MC_FLOOR = PROP_PREFIX + 
	"MixtureComponentScoreFloor";


    /**
     * Mixture component score floor default value.
     */
    public final static float PROP_MC_FLOOR_DEFAULT = 0.0f;


    /**
     * Variance floor.
     */
    public final static String PROP_VARIANCE_FLOOR = PROP_PREFIX + 
	"varianceFloor";


    /**
     * Variance floor default value.
     */
    public final static float PROP_VARIANCE_FLOOR_DEFAULT = 0.0001f;


    /**
     * Mixture weight floor.
     */
    public final static String PROP_MW_FLOOR = PROP_PREFIX + 
	"mixtureWeightFloor";


    /**
     * Mixture weight floor default value.
     */
    public final static float PROP_MW_FLOOR_DEFAULT = 1e-7f;


    /**
     * Transition probability floor.
     */
    public final static String PROP_TP_FLOOR = PROP_PREFIX + 
	"transitionProbabilityFloor";


    /**
     * transition probability floor default value.
     */
    public final static float PROP_TP_FLOOR_DEFAULT = 0.0001f;


    /**
     * Model load timer
     */
    protected final static String TIMER_LOAD = "AM_Load";


    /**
     * The logger for this class
     */
    private static Logger logger = 
	    Logger.getLogger(PROP_PREFIX + "AcousticModel");


    protected String name;
    protected String context;
    protected Loader loader;
    private boolean useComposites = false;

    transient protected SphinxProperties props;
    transient protected Timer loadTimer;
    transient private Map compositeSenoneSequenceCache = new HashMap();


     /**
      * Initializes an acoustic model of a given context. This method
      * should be called once per context. It is used to associate a
      * particular context with an acoustic model resource.  This
      * method should be called only when one acoustic model is
      * specified in the properties file. Otherwise, use the method
      * <code>getAcousticModel(name, context)</code>.
      *
      * @param context	the context of interest
      *
      * @return the acoustic model associated with the context or null
      * if the given context has no associated acoustic model
      *
      * @throws IOException if the model could not be loaded
      * @throws FileNotFoundException if the model does not exist
      */
    public static AcousticModel getAcousticModel(String context) 
	throws IOException, FileNotFoundException {
	
	// This bit of code simply checks if there are any acoustic
	// model names specified in the props file. If there is one
	// name, use that name. If there are more than one, then flag
	// an error.
	
	SphinxProperties props = 
	    SphinxProperties.getSphinxProperties(context);
	String amNames = props.getString(PROP_NAMES, PROP_NAMES_DEFAULT);
	if (amNames != null) {
	    StringTokenizer tokenizer = new StringTokenizer(amNames);
	    
	    if (tokenizer.countTokens() == 0) {
		amNames = null;
	    } else if (tokenizer.countTokens() == 1) {
		amNames = amNames.trim();
	    } else if (tokenizer.countTokens() > 1) {
		throw new Error
		    ("AcousticModel: more than one acoustic model specified. "+
		     "Instead of method getAcousticModel(context), " +
		     "use method getAcousticModel(name, context).");
	    }
	}
	return getAcousticModel(amNames, context);
    }
    
    
    /**
     * Returns the acoustic model of the given name and context.
     * If the acoustic model of the given name and context has not
     * been loaded, it will be loaded, and returned.
     * If there is only one acoustic model for this context,
     * "name" can be null.
     *
     * @param name  the name of the acoustic model, or null if
     *    the acoustic model has no name.
     * @param context  the context of interest
     *
     * @return the name acoustic model in the given context, or
     *   null if no such acoustic model is found
     *
     * @throws IOException if the model count not be loaded
     * @throws FileNotFoundException if the model does not exist
     */
    public static AcousticModel getAcousticModel(String name, String context) 
	throws IOException, FileNotFoundException {
	String key = getModelKey(name, context);
	if (contextMap.get(key) == null) {
	    AcousticModel model = new AcousticModel(name, context);
	    contextMap.put(key, model);
	}
	return (AcousticModel) contextMap.get(key);
    }


    /**
     * Returns the key into the AcousticModel contextMap given
     * the name and context of the AcousticModel.
     *
     * @param name  the name of the AcousticModel
     * @param context  the context of interest
     *
     * @return the key
     */
    protected static String getModelKey(String name, String context) {
	if (name == null) {
	    return context;
	} else {
	    return (context + "." + name);
	}
    }


    /**
     * Return the names of all acoustic models in the given context.
     *
     * @param context the context of interest
     *
     * @return a list of all names of acoustic models in the given context;
     *    if there are no names, it will return a list with no elements.
     */
    public static List getNames(String context) {
	List nameList = new LinkedList();
	SphinxProperties props = SphinxProperties.getSphinxProperties(context);
	if (props != null) {
	    String names = props.getString(PROP_NAMES, PROP_NAMES_DEFAULT);
	    if (names != null) {
		StringTokenizer tokenizer = new StringTokenizer(names);
		while (tokenizer.hasMoreTokens()) {
		    String name = tokenizer.nextToken();
		    if (name.length() > 0) {
			nameList.add(name);
		    }
		}
	    }
	}
	return nameList;
    }


    /**
     * Creates an acoustic model with the given name and context.
     * Since acoustic models are only created by the factory method,
     * getAcousticModel(), this constructor is private.
     *
     * @param name the name of the acoustic model
     * @param context the context for this acoustic model
     *
     * @throws IOException if the model could not be loaded
     * @throws FileNotFoundException if the model does not exist
     *
     * @see #getAcousticModel
     */
    private AcousticModel(String name, String context)
 	throws IOException, FileNotFoundException {
	this.name = name;
	this.context = context;
        this.props = SphinxProperties.getSphinxProperties(context);
        this.loadTimer = Timer.getTimer(context, TIMER_LOAD);
        this.useComposites = props.getBoolean(PROP_USE_COMPOSITES,
					      PROP_USE_COMPOSITES_DEFAULT);

        loadTimer.start();
        load();
        loadTimer.stop();
        logInfo();
    }


    /**
     * Creates an acoustic model. Since acoustic models are only
     * created by the factory method <code> getAcousticModel </code>,
     * this contructor is <code> private </code>. This constructor
     * is used when there is only one acoustic model for the given
     * context, which is why the acoustic model has no name.
     * Note that an acoustic model can have a name even if it is
     * the only acoustic model in this context.
     *
     * @param context 	the context for this acoustic model
     *
     * @throws IOException if the model could not be loaded
     * @throws FileNotFoundException if the model does not exist
     *
     * @see #getAcousticModel
     */
    private AcousticModel(String context) 
	throws IOException, FileNotFoundException {
	this(null, context);
    }
    
    /**
     * Null constructor, just because we have another class that
     * extends this one.
     */
    protected AcousticModel() {
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
      * Given a unit, returns the HMM that best matches the given unit.
      * If an exact match is not found, then different word positions
      * are used. If any of the contexts are non-silence filler units.
      * a silence filler unit is tried instead.
      *
      * @param unit 		the unit of interest
      * @param position 	the position of the unit of interest
      * 			acceptable.
      *
      * @return 	the HMM that best matches, or null if no match
      * 		could be found.
      */
     public HMM lookupNearestHMM(Unit unit, HMMPosition position) {
	 return lookupNearestHMM(unit, position, false);
     }


     /**
      * Gets a composite HMM for the given unit and context
      *
      * @param unit the unit for the hmm
      * @param position the position of the unit within the word
      *
      * @return a composite HMM
      */
     private HMM getCompositeHMM(Unit unit, HMMPosition position) {
	 Unit ciUnit = Unit.getUnit(unit.getName(),
		 unit.isFiller(), Context.EMPTY_CONTEXT);

	 SenoneSequence compositeSequence = getCompositeSenoneSequence(unit);

	 HMM contextIndependentHMM = lookupNearestHMM(ciUnit,
		 HMMPosition.UNDEFINED, true);
	 float[][] tmat = contextIndependentHMM.getTransitionMatrix();
	 return new HMM(unit, compositeSequence, tmat, position);
     }


     /**
      * Given a unit, returns the HMM that best matches the given unit.
      * If exactMatch is false and an exact match is not found, 
      * then different word positions
      * are used. If any of the contexts are non-silence filler units.
      * a silence filler unit is tried instead.
      *
      * @param unit 		the unit of interest
      * @param position 	the position of the unit of interest
      * @param exactMatch 	if true, only an exact match is
      * 			acceptable.
      *
      * @return 	the HMM that best matches, or null if no match
      * 		could be found.
      */
     public HMM lookupNearestHMM(Unit unit, HMMPosition position,
	     boolean exactMatch) {

	 if (exactMatch) {
	     return lookupHMM(unit, position);
	 } else {
	     HMMManager mgr = loader.getHMMManager();
	     HMM hmm = mgr.get(position, unit);

	     if (hmm != null) {
	  // System.out.println("EXACT match for "  + unit + " at " +
	  // position + " hmm is " + hmm);
		 return hmm;
	     }
	     // no match, try a composite

	     if (useComposites && hmm == null) {
		 if (isComposite(unit)) {
		     hmm = getCompositeHMM(unit, position);
		     if (hmm != null) {
			 //System.out.println("Adding composite unit " + hmm);
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
      *
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
      *
      * @return the unit or null if the unit was not found
      */
     public Unit lookupUnit(String name) {
	 return (Unit) loader.getContextIndependentUnits().get(name);
     }

     /**
      * Returns an iterator that can be used to iterate through all
      * the HMMs of the acoustic model
      *
      * @return an iterator that can be used to iterate through all
      * HMMs in the model. The iterator returns objects of type
      * <code>HMM</code>.
      */
     public Iterator getHMMIterator() {
	 return loader.getHMMManager().getIterator();
      }


     /**
      * Returns an iterator that can be used to iterate through all
      * the CI units in the acoustic model
      *
      * @return an iterator that can be used to iterate through all
      * CI units. The iterator returns objects of type
      * <code>Unit</code>
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
      *
      */
     public SenoneSequence getCompositeSenoneSequence(Unit unit) {
	 Context context = unit.getContext();
	 SenoneSequence compositeSenoneSequence = null;
	 compositeSenoneSequence = (SenoneSequence)
	     compositeSenoneSequenceCache.get(unit.toString());

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

	 List senoneSequenceList = new ArrayList();

	// collect all senone sequences that match the pattern
	 for (Iterator i = getHMMIterator(); i.hasNext(); ) {
	     HMM hmm = (HMM) i.next();
	     Unit hmmUnit = hmm.getUnit();
	     if (hmmUnit.isPartialMatch(unit.getName(), context)) {
		 if (logger.isLoggable(Level.FINE)) {
		    logger.fine("collected: " + hmm.getUnit().toString());
		 }
		 senoneSequenceList.add(hmm.getSenoneSequence());
	     }
	 }

	 // couldn't find any matches, so at least include the CI unit
	 if (senoneSequenceList.size() == 0) {
	    Unit ciUnit = Unit.getUnit(unit.getName(), unit.isFiller());
	     HMM baseHMM = lookupHMM(ciUnit, HMMPosition.UNDEFINED);
	     senoneSequenceList.add(baseHMM.getSenoneSequence());
	 }

	 // Add this point we have all of the senone sequences that
	 // match the base/context pattern collected into the list.
	 // Next we build a CompositeSenone consisting of all of the
	 // senones in each position of the list.

	 // First find the longest senone sequence

	 int longestSequence = 0;
	 for (int i = 0; i < senoneSequenceList.size(); i++) {
	     SenoneSequence ss = (SenoneSequence) senoneSequenceList.get(i);
	     if (ss.getSenones().length > longestSequence) {
		 longestSequence = ss.getSenones().length;
	     }
	 }

	 // now collect all of the senones at each position into
	 // arrays so we can create CompositeSenones from them
	 // QUESTION: is is possible to have different size senone
	 // sequences. For now lets assume the worst case.

	 List compositeSenones = new ArrayList();
	 for (int i = 0; i < longestSequence; i++) {
	     Set compositeSenoneSet = new HashSet();
	     for (int j = 0; j < senoneSequenceList.size(); j++) {
		 SenoneSequence senoneSequence = 
		     (SenoneSequence) senoneSequenceList.get(j);
		 if (i < senoneSequence.getSenones().length) {
		     Senone senone = senoneSequence.getSenones()[i];
		     compositeSenoneSet.add(senone);
		 }
	     }
	     compositeSenones.add(CompositeSenone.create(compositeSenoneSet));
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
     * Returns the properties of this AcousticModel.
     *
     * @return the properties of this AcousticModel, or null if no properties
     */
    public SphinxProperties getProperties() {
        return loader.getModelProperties();
    }

    
    /**
     * Given a unit, returns the HMM that exactly matches the given
     * unit.  
     *
     * @param unit 		the unit of interest
     * @param position 	the position of the unit of interest
     *
     * @return 	the HMM that exactly matches, or null if no match
     * 		could be found.
     */
    private HMM lookupHMM(Unit unit, HMMPosition position) {
        return loader.getHMMManager().get(position, unit);
    }
    
    
    /**
     * Creates a string useful for tagging a composite senone sequence
     *
     * @param base the base unit
     * @param context the context
     *
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
     * Loads the acoustic model
     *
     * @throws IOException if the model could not be loaded
     * @throws FileNotFoundException if the model does not exist
     */
    protected void load() throws IOException, FileNotFoundException {
	String formatProp = PROP_FORMAT;
	if (name != null) {
	    formatProp = PROP_PREFIX + name + ".format";
	}
        String format = props.getString(formatProp, PROP_FORMAT_DEFAULT);

        if (format.equals("sphinx3.ascii")) {
	    logger.info("Sphinx 3 ASCII format");
            loader = new Sphinx3Loader(name, props, false);
        } else if (format.equals("sphinx3.binary")) {
	    logger.info("Sphinx 3 binary format");
            loader = new Sphinx3Loader(name, props, true);
        }  else if (format.equals("sphinx4.ascii")) {
	    logger.info("Sphinx 4 ASCII format");
            loader = new Sphinx4Loader(name, props, false);
        } else if (format.equals("sphinx4.binary")) {
	    logger.info("Sphinx 4 binary format");
            loader = new Sphinx4Loader(name, props, true);
	    }  else { // add new loading code here.
            loader = null;
            logger.severe("Unsupported acoustic model format " + format);
	}
    }

    
    /**
     * Dumps information about this model to the logger
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
     *
     * @return hmm the hmm or null if it was not found
     */
    private HMM getHMMAtAnyPosition(Unit unit) {
        HMM hmm = null;
        HMMManager mgr = loader.getHMMManager();
        for (Iterator i = HMMPosition.iterator(); 
             hmm == null && i.hasNext(); ) {
            HMMPosition pos = (HMMPosition) i.next();
            hmm = mgr.get(pos, unit);
        }
        return hmm;
    }
    
    /**
     * Given a unit, search for the HMM associated with this unit by
     * replacing all non-silence filler contexts with the silence
     * filler context
     *
     * @param unit the unit of interest
     *
     * @return the associated hmm or null
     */
    private HMM getHMMInSilenceContext(Unit unit, HMMPosition position) {
        HMM hmm = null;
        HMMManager mgr = loader.getHMMManager();
        Context context = unit.getContext();
        
        if (context instanceof LeftRightContext) {
            LeftRightContext lrContext = (LeftRightContext) context;
            
            Unit[] lc  = lrContext.getLeftContext();
            Unit[] rc  = lrContext.getRightContext();
            
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
                Context newContext =  LeftRightContext.get(nlc, nrc);
                Unit newUnit = Unit.getUnit(unit.getName(),
                        unit.isFiller(), newContext);
                hmm = mgr.get(position, newUnit);
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
     * @param c the context to check
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
     *
     * @return true if the array contains a filler that is not the
     * silence filler
     */
    private boolean hasNonSilenceFiller(Unit[] units) {
	if (units == null) {
	    return false;
	}

        for (int i = 0; i < units.length; i++) {
            if (units[i].isFiller() &&
                !units[i].equals(Unit.SILENCE)) {
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
     *
     * @return true if the array contains a filler that is not the
     * silence filler
     */
    private Unit[] replaceNonSilenceFillerWithSilence(Unit[] context) {
        Unit[] replacementContext = new Unit[context.length];
        for (int i = 0; i < context.length; i++) {
            if (context[i].isFiller() && !context[i].equals(Unit.SILENCE)) {
                replacementContext[i] = Unit.SILENCE;
            } else {
                replacementContext[i] = context[i];
            }
        }
        return replacementContext;
    }
}

