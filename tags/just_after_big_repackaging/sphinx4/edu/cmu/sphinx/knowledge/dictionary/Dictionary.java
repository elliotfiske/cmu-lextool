
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

package edu.cmu.sphinx.knowledge.dictionary;

import java.util.List;

/**
 * Provides a generic interface to a dictionary. The dictionary is
 * responsibile for determining how a word is pronounced.
 */
public interface Dictionary {

    /**
     * The name of the SphinxProperty that specifies whether to look
     * for the Dictionaries at the Acoustic Model location.
     */
    public static final String PROP_USE_AM_LOCATION =
    "edu.cmu.sphinx.knowledge.dictionary.Dictionary.useAcousticModelLocation";


    /**
     * The name of the SphinxProperty for the dictionary file path.
     */
    public static final String PROP_DICTIONARY =
    "edu.cmu.sphinx.knowledge.dictionary.Dictionary.dictionaryPath";


    /**
     * The name of the SphinxProperty for the filler dictionary file path.
     */
    public static final String PROP_FILLER_DICTIONARY =
    "edu.cmu.sphinx.knowledge.dictionary.Dictionary.fillerPath";


    /**
     * The name of the SphinxProperty that specifies whether to add
     * a duplicate SIL-ending pronunication.
     */
    public static final String PROP_ADD_SIL_ENDING_PRONUNCIATION =
    "edu.cmu.sphinx.knowledge.dictionary.Dictionary.addSilEndingPronunciation";

    /**
     * The name of the SphinxProperty that specifies the word to
     * substitute when a lookup fails to find the word in the
     * dictionary.  If this is not set, no substitute is performed.
     */
    public static final String PROP_WORD_REPLACEMENT =
    "edu.cmu.sphinx.knowledge.dictionary.Dictionary.wordReplacement";

    /**
     * The name of the SphinxProperty that specifies whether the
     * dictionary should return null if a word is not found in the
     * dictionary, or whether it should throw an error.  If true, a
     * null is returned for words that are not found in the dictionary
     * (and the 'PROP_WORD_REPLACEMENT' property is not set).
     */
    public static final String PROP_ALLOW_MISSING_WORDS =
    "edu.cmu.sphinx.knowledge.dictionary.Dictionary.allowMissingWords";
    
    /**
     * Retrieves the pronunciations for a particular word based upon
     * its classification.
     *
     * @param text the spelling of the word of interest.
     * @param wordClassification the classification of the word
     * (typically part of speech classification)  or null if all word
     * classifications are acceptable. The word classification must be
     * one of the set returned by <code>
     * getPossibleWordClassifications </code>
     *
     * @return an array of zero or more
     * <code>Pronunciation</code> objects.
     *
     * @see edu.cmu.sphinx.knowledge.dictionary.Pronunciation
     */
    public Pronunciation[] getPronunciations(String text,
	    edu.cmu.sphinx.knowledge.dictionary.WordClassification wordClassification);

    /**
     * Retrieves the pronunciations for a particular word based upon
     * its classification and tag.
     *
     * @param text the spelling of the word of interest.
     * @param wordClassification the classification of the word
     * (typically part of speech classification)  or null if all word
     * classifications are acceptable. The word classification must be
     * one of the set returned by <code>
     * getPossibleWordClassifications </code>
     * @param tag a tag used to distinguish one homograph from
     * another.
     *
     * @return an array of zero or more
     * <code>Pronunciation</code> objects.
     *
     * @see edu.cmu.sphinx.knowledge.dictionary.Pronunciation
     */
    public Pronunciation[] getPronunciations(String text,
	    edu.cmu.sphinx.knowledge.dictionary.WordClassification wordClassification, String tag);


    /**
     * Returns the set of all possible word classifications for this
     * dictionary.
     *
     * @return the set of all possible word classifications
     */
    public edu.cmu.sphinx.knowledge.dictionary.WordClassification[] getPossibleWordClassifications();



    /**
     * Dumps out a dictionary
     *
     */
    public void dump();

}
