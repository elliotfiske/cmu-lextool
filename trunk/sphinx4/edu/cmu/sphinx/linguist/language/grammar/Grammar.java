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

package edu.cmu.sphinx.linguist.language.grammar;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Random;
import java.util.Set;

import edu.cmu.sphinx.linguist.dictionary.Dictionary;
import edu.cmu.sphinx.linguist.dictionary.Word;
import edu.cmu.sphinx.linguist.language.ngram.LanguageModel;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.SphinxProperties;


/**
 * Classes that implement this interface create grammars. A grammar
 * is represented internally as a graph of {@link GrammarNode GrammarNodes}
 * linked together by {@link GrammarArc GrammarArcs}. Calling
 * {@link #getInitialNode() getInitialNode} will return the first node
 * of the grammar graph. To traverse the grammar graph, one should
 * call GrammarNode.getSuccessors, which will return an array of GrammarArcs,
 * from which you can reach the neighboring GrammarNodes.
 * <p>
 * Note that all grammar probabilities are maintained in LogMath log
 * domain.
 */
public abstract class  Grammar {
    /**
     * Prefix for search.Linguist SphinxProperties.
     */
    public final static String PROP_PREFIX =
	"edu.cmu.sphinx.linguist.language.grammar.Grammar.";

    /**
     * Property to control the the dumping of the grammar
     */
    public final static String PROP_SHOW_GRAMMAR 
        = PROP_PREFIX + "showGrammar";
    /**
     * The default value for PROP_SHOW_GRAMMAR.
     */
    public final static boolean PROP_SHOW_GRAMMAR_DEFAULT = false;

    /**
     * Property to control whether grammars are optimized or not
     */
    public final static String PROP_OPTIMIZE_GRAMMAR 
        = PROP_PREFIX + "optimizeGrammar";

    /**
     * The default value for PROP_OPTIMIZE_GRAMMAR
     */
    public final static boolean PROP_OPTIMIZE_GRAMMAR_DEFAULT = true;

    private GrammarNode initialNode;
    private Set grammarNodes;
    private Dictionary dictionary;
    private LanguageModel languageModel;
    private LogMath logMath;
    protected SphinxProperties props;
    private boolean showGrammar; 	
    private boolean optimizeGrammar = true; 	
    private final static Word[][] EMPTY_ALTERNATIVE = new Word[0][0];
    private Random randomizer = new Random();


    /**
     * Initializes a grammar using the given dictionary
     *
     * @param context the name of the sphinx context
     *
     * @param languageModel the language model
     * @param dictionary the dictionary used to get word
     * pronunciations
     *
     * @throws java.io.IOException if the grammar could not be loaded
     */

    public void initialize(String context,
                           LanguageModel languageModel, Dictionary dictionary)
			throws IOException, NoSuchMethodException {
	if (dictionary == null) {
            throw new Error("Dictionary is null!");
	}

        this.languageModel = languageModel;
	this.props = SphinxProperties.getSphinxProperties(context);
	this.dictionary = dictionary;
        this.logMath = LogMath.getLogMath(context);
	this.showGrammar = props.getBoolean(PROP_SHOW_GRAMMAR, 
			PROP_SHOW_GRAMMAR_DEFAULT);
	this.optimizeGrammar = props.getBoolean(PROP_OPTIMIZE_GRAMMAR, 
			PROP_OPTIMIZE_GRAMMAR_DEFAULT);
	grammarNodes = new HashSet();
	initialNode = createGrammar();

        if (optimizeGrammar) {
            optimizeGrammar();
        }

        if (showGrammar) {
            dumpGrammar("grammar.gdl");
            dumpRandomSentences("sentences.txt", 100);
        }
	this.dictionary = null;
    }

    /**
     * Initializes a grammar using the default dictionary and no
     * language model.
     *
     * @param context the name of the sphinx context
     *
     * @throws java.io.IOException if the grammar could not be loaded
     */
    public void initialize(String context)
	throws IOException, NoSuchMethodException {
	initialize(context, (LanguageModel) null, (Dictionary) null);
    }

    /**
     * Initializes a grammar using the given reference sentence
     *
     * @param context the name of the sphinx context
     *
     * @param dictionary the dictionary used to get word
     * pronunciations
     *
     * @param referenceText the reference sentence used to define the
     * grammar
     *
     * @throws java.io.IOException if the grammar could not be loaded
     */

    public void initialize(String context, Dictionary dictionary,
			   String referenceText)
			throws IOException, NoSuchMethodException {
	if (dictionary == null) {
            throw new Error("Dictionary is null!");
	}
	this.props = SphinxProperties.getSphinxProperties(context);
	this.showGrammar = props.getBoolean(PROP_SHOW_GRAMMAR, 
			PROP_SHOW_GRAMMAR_DEFAULT);
	this.optimizeGrammar = props.getBoolean(PROP_OPTIMIZE_GRAMMAR, 
			PROP_OPTIMIZE_GRAMMAR_DEFAULT);
	this.dictionary = dictionary;
	grammarNodes = new HashSet();
	initialNode = createGrammar(referenceText);

        if (optimizeGrammar) {
            optimizeGrammar();
        }

        if (showGrammar) {
            dumpGrammar("grammar.gdl");
            dumpRandomSentences("sentences.txt", 100);
        }
	this.dictionary = null;
    }

    /**
     * Retrievs log base used to represent probabilities in this grammar
     *
     * @return the log math representing the log base for the gramamr
     */
    public LogMath getLogMath() {
        return logMath;
    }

    /**
     * Returns the initial node for the grammar
     *
     * @return the initial grammar node
     */
    public GrammarNode getInitialNode() {
	return initialNode;
    }

    /**
     * Dumps statistics for this grammar
     *
     */
    public void dumpStatistics() {
        int successorCount = 0;
        System.out.println("Num nodes : " +  getNumNodes());
        for (Iterator i = grammarNodes.iterator(); i.hasNext(); ) {
            GrammarNode node = (GrammarNode) i.next();
            successorCount += node.getSuccessors().length;
        }
        System.out.println("Num arcs  : " + successorCount);
        System.out.println("Avg arcs  : " +
                ((float) successorCount / getNumNodes()));
    }


    /**
     * Dump a set of random sentences that fit this grammar
     *
     * @param path the name of the file to dump the sentences to
     * @param count dumps no more than this. May dump less than this
     * depending upon the number of uniqe sentences in the grammar.
     */
    public void dumpRandomSentences(String path, int count) {
        try {
            Set set = new HashSet();
            PrintWriter out = new PrintWriter(new FileOutputStream(path));
            for (int i = 0; i < count; i++) {
                String s = getRandomSentence();
                if (!set.contains(s)) {
                    set.add(s);
                    out.println(s);
                }
            }
            out.close();
        } catch (IOException ioe) {
            System.out.println("Can't write random sentences to " +
                    path + " " + ioe);
        }
    }

    /**
     * Returns a random sentence that fits this grammar
     *
     * @return a random sentence that fits this grammar
     */
    public String getRandomSentence() {
        StringBuffer sb = new StringBuffer();
        GrammarNode node = getInitialNode();
        while (!node.isFinalNode()) {
            if (!node.isEmpty()) {
                Word word = node.getWord();
                if (!word.isFiller()) {
                    if (sb.length() > 0) {
                        sb.append(" ");
                    }
                    sb.append(word.getSpelling());
                }
            }
            node = selectRandomSuccessor(node);
        }
        return sb.toString();
    }


    /**
     * Given a node, select a random successor from the set of
     * possible successor nodes
     *
     * @param node the node
     * @return a random successor node.
     */
    private GrammarNode selectRandomSuccessor(GrammarNode node) {
        GrammarArc[] arcs = node.getSuccessors();
        int index = randomizer.nextInt(arcs.length);
        return arcs[index].getGrammarNode();
    }

    /**
     * Returns the dictionary used by this grammar
     *
     * @return the dictionary
     *
     */
    protected Dictionary getDictionary() {
	return dictionary;
    }


    /**
     * Returns the language model for the grammar
     *
     * @return the language model (null if there is no language model)
     */
    protected LanguageModel getLanguageModel() {
        return languageModel;
    }

    /**
     * Dumps the grammar
     */
    public void dumpGrammar(String name) {
	initialNode.dumpGDL(name);
    }

    /**
     * returns the number of nodes in this grammar
     *
     * @return the number of nodes
     */
    public int getNumNodes() {
	return grammarNodes.size();
    }


    /**
     * returns the set of of nodes in this grammar
     *
     * @return the set of nodes
     */
    public Set getGrammarNodes() {
	return grammarNodes;
    }




    /**
     * Creates a grammar. Subclasses of
     * grammar should implement this method.
     *
     * @return the initial node for the grammar
     *
     * @throws java.io.IOException if the grammar could not be loaded
     * @throws java.lang.NoSuchMethodException if called with inappropriate subclass.
     */
    protected abstract GrammarNode createGrammar()
	throws IOException, NoSuchMethodException;

    /**
     * Create class from reference text (not implemented).
     *
     * @param bogusText dummy variable
     *
     * @throws NoSuchMethogException if called with reference sentence
     */
    protected GrammarNode createGrammar(String bogusText)
	throws NoSuchMethodException {
	throw new NoSuchMethodException("Does not create "
				       + "grammar with reference text");
    }

    /**
     * Returns a new GrammarNode with the given set of alternatives.
     *
     * @param identity the id for this node
     * @param alts the set of alternative word lists for this GrammarNode
     */
    protected GrammarNode createGrammarNode(int identity, String[][] alts) {
	GrammarNode node;
        Word[][] alternatives = new Word[alts.length][];

	for (int i = 0; i < alternatives.length; i++) {
	    alternatives[i] = new Word[alts[i].length];
	    for (int j = 0; j < alts[i].length; j++) {

                Word word = getDictionary().getWord(alts[i][j]);
		// Pronunciation[] pronunciation = word.getPronunciations(null);

		if (word == null) {
		    alternatives = EMPTY_ALTERNATIVE;
		    break;
		} else {
		    alternatives[i][j] = word;
                }
	    }
	}
            
        node = new GrammarNode(identity, alternatives);
	add(node);
	return node;
    }

    /**
     * Returns a new GrammarNode with the given single word. If the
     * word is not in the dictionary, an empty node is created
     *
     * @param identity the id for this node
     * @param word the word for this grammar node
     */
    protected GrammarNode createGrammarNode(int identity, String word) {
	GrammarNode node = null;
        Word[][] alternatives = EMPTY_ALTERNATIVE;
        Word wordObject = getDictionary().getWord(word);
	// Pronunciation[] pronunciation = wordObject.getPronunciations(null);

	if (wordObject != null) {
	    alternatives = new Word[1][];
	    alternatives[0] = new Word[1];
	    alternatives[0][0] = wordObject;
            node = new GrammarNode(identity, alternatives);
            add(node);
	} else {
            node = createGrammarNode(identity, false);
            System.out.println("Can't find pronunciation for " + word);
        }
	return node;
    }

    /**
     * Creates a grammar node in this grammar with the given identity
     *
     * @param identity the identity of the node
     * @param isFinal if true, this is a final node
     *
     * @return the grammar node
     */
    protected GrammarNode createGrammarNode(int identity, boolean isFinal) {
	GrammarNode node;
        node = new GrammarNode(identity, isFinal);
	add(node);
	return node;
    }


    /**
     * Adds the given grammar node to the set of nodes for this
     * grammar
     *
     * @param node the grammar node
     */
    private void add(GrammarNode node) {
	grammarNodes.add(node);
    }

    /**
     * Eliminate unnecessary nodes from the grammar.  This method goes
     * through the grammar and looks for branches to nodes that have
     * no words and have only a single exit and bypasses these nodes.
     *
     */
    private void optimizeGrammar() {
        Set nodes = getGrammarNodes();
        for (Iterator i = nodes.iterator(); i.hasNext(); ) {
            GrammarNode g = (GrammarNode) i.next();
            g.optimize();
        }
    }

}
