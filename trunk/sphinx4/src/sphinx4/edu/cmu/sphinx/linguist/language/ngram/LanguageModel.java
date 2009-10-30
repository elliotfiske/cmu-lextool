/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.linguist.language.ngram;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Dictionary;
import edu.cmu.sphinx.util.props.*;

import java.io.IOException;
import java.util.Set;
import java.util.logging.Logger;

/**
 * Represents the generic interface to an N-Gram language model.
 * <p/>
 * Note that all probabilities are in LogMath log base, except as otherwise noted.
 */
@SuppressWarnings({"JavaDoc"})
public interface LanguageModel extends Configurable {

    /** The SphinxProperty specifying the format of the language model. */
    @S4String(defaultValue = "arpa")
    public final static String PROP_FORMAT = "format";

    /** The Sphinx Property specifying the location of the language model. */
    @S4String(defaultValue = ".")
    public final static String PROP_LOCATION = "location";

    /** The Sphinx Property specifying the unigram weight */
    @S4Double(defaultValue = 1.0)
    public final static String PROP_UNIGRAM_WEIGHT = "unigramWeight";
    /**
     * The Sphinx Property specifying the maximum depth reported by the language model (from a getMaxDepth()) call. If
     * this property is set to (-1) (the default) the language model reports the implicit depth of the model. This
     * property allows a deeper language model to be used. For instance, a trigram language model could be used as a
     * bigram model by setting this property to 2. Note if this property is set to a value greater than the implicit
     * depth, the implicit depth is used. Legal values for this property are 1..N and -1.
     */
    @S4Integer(defaultValue = -1)
    public final static String PROP_MAX_DEPTH = "maxDepth";

    /** The Sphinx Property specifying the dictionary to use */
    @S4Component(type = Dictionary.class)
    public final static String PROP_DICTIONARY = "dictionary";


    /** Create the language model
     * @throws java.io.IOException*/
    public void allocate() throws IOException;


    /** Deallocate resources allocated to this language model */
    public void deallocate();


    /** Called before a recognition */
    public void start();


    /** Called after a recognition */
    public void stop();


    /**
     * Gets the ngram probability of the word sequence represented by the word list
     *
     * @param wordSequence the wordSequence
     * @return the probability of the word sequence in LogMath log base
     */
    public float getProbability(WordSequence wordSequence);


    /**
     * Gets the smear term for the given wordSequence
     *
     * @param wordSequence the word sequence
     * @return the smear term associated with this word sequence
     */
    public float getSmear(WordSequence wordSequence);


    /**
     * Returns the set of words in the lanaguage model. The set is unmodifiable.
     *
     * @return the unmodifiable set of words
     */
    public Set<String> getVocabulary();


    /**
     * Returns the maximum depth of the language model
     *
     * @return the maximum depth of the language mdoel
     */
    public int getMaxDepth();


    /**
     * Used for reporting errors and warnings during loading
     *
     * @return the logger used by the LanguageModel
     */
    public Logger getLogger();

}
