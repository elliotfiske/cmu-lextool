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

package edu.cmu.sphinx.decoder.search;


// a test search manager.

import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.result.Lattice;
import edu.cmu.sphinx.result.LatticeOptimizer;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.StatisticsVariable;
import edu.cmu.sphinx.util.Timer;
import edu.cmu.sphinx.decoder.search.Pruner;
import edu.cmu.sphinx.decoder.search.SearchManager;
import edu.cmu.sphinx.decoder.search.Token;
import edu.cmu.sphinx.decoder.search.ActiveList;
import edu.cmu.sphinx.decoder.scorer.AcousticScorer;
import edu.cmu.sphinx.decoder.linguist.Linguist;
import edu.cmu.sphinx.decoder.linguist.SearchState;
import edu.cmu.sphinx.decoder.linguist.SearchStateArc;
import edu.cmu.sphinx.decoder.linguist.WordSearchState;
import edu.cmu.sphinx.decoder.linguist.HMMSearchState;
import edu.cmu.sphinx.decoder.linguist.lextree.LexTreeLinguist;

import edu.cmu.sphinx.knowledge.acoustic.HMM;
import edu.cmu.sphinx.knowledge.acoustic.HMMState;
import edu.cmu.sphinx.knowledge.language.WordSequence;
import edu.cmu.sphinx.knowledge.dictionary.Word;

import java.util.*;



/**
 * Provides the breadth first search. To perform recognition
 * an application should call initialize before
 * recognition begins, and repeatedly call <code> recognize </code>
 * until Result.isFinal() returns true.  Once a final result has been
 * obtained, <code> terminate </code> should be called.
 *
 * All scores and probabilities are maintained in the log math log
 * domain.
 */

public class WordPruningBreadthFirstSearchManager implements  SearchManager {

    private final static String PROP_PREFIX =
            "edu.cmu.sphinx.decoder.search.BreadthFirstSearchManager.";

    /**
     * Sphinx property that defines the type of active list to use
     */
    public final static String PROP_ACTIVE_LIST_TYPE =
            PROP_PREFIX + "activeListType";

    /**
     * The default value for the PROP_ACTIVE_LIST_TYPE property
     */
    public final static String PROP_ACTIVE_LIST_TYPE_DEFAULT =
            "edu.cmu.sphinx.decoder.search.SimpleActiveList";


    /**
     * A sphinx property than, when set to <code>true</code> will
     * cause the recognizer to count up all the tokens in the active
     * list after every frame.
     */
    public final static String PROP_SHOW_TOKEN_COUNT =
            PROP_PREFIX + "showTokenCount";

    /**
     * The default value for the PROP_SHOW_TOKEN_COUNT property
     */
    public final static boolean PROP_SHOW_TOKEN_COUNT_DEFAULT = false;


    /**
     * Sphinx property for checking if the order of states is valid.
     */
    public final static String PROP_CHECK_STATE_ORDER = 
        PROP_PREFIX + "checkStateOrder";

    /**
     * The default value of the PROP_CHECK_STATE_ORDER property.
     */
    public final static boolean PROP_CHECK_STATE_ORDER_DEFAULT = false;

    /**
     * Sphinx property that specifies whether to build a word lattice.
     */
    public final static String PROP_BUILD_WORD_LATTICE =
        PROP_PREFIX + "buildWordLattice";

    /**
     * The default value of the PROP_BUILD_WORD_LATTICE property.
     */
    public final static boolean PROP_BUILD_WORD_LATTICE_DEFAULT = true;
    

    private Linguist linguist;		// Provides grammar/language info
    private Pruner pruner;		// used to prune the active list
    private AcousticScorer scorer;	// used to score the active list

    private int currentFrameNumber;	// the current frame number
    private ActiveList activeList;	// the list of active tokens
    private ActiveListManager activeBucket;
    private List resultList;		// the current set of results
    private SphinxProperties props;	// the sphinx properties
    private LogMath logMath;

    private Timer scoreTimer;
    private Timer pruneTimer;
    private Timer growTimer;

    private StatisticsVariable totalTokensScored;
    private StatisticsVariable curTokensScored;
    private StatisticsVariable tokensCreated;

    private boolean showTokenCount;
    private boolean checkStateOrder;
    private boolean buildWordLattice;
    private boolean allowSinglePathThroughHMM = false;

    private Map bestTokenMap;
    private AlternateHypothesisManager loserManager;
    private Class[] stateOrder;


    private boolean wantTokenStacks = false;
    private int maxTokenHeapSize = 3;


    /**
     * Initializes this BreadthFirstSearchManager with the given
     * context, linguist, scorer and pruner.
     *
     * @param context the context to use
     * @param linguist the Linguist to use
     * @param scorer the AcousticScorer to use
     * @param pruner the Pruner to use
     */
    public void initialize(String context, Linguist linguist,
                           AcousticScorer scorer, Pruner pruner) {
        this.linguist = linguist;
        this.scorer = scorer;
        this.pruner = pruner;
        this.props = SphinxProperties.getSphinxProperties(context);
        this.logMath = LogMath.getLogMath(context);

        scoreTimer = Timer.getTimer(context, "Score");
        pruneTimer = Timer.getTimer(context, "Prune");
        growTimer = Timer.getTimer(context, "Grow");

        totalTokensScored = StatisticsVariable.getStatisticsVariable(
                props.getContext(), "totalTokensScored");
        curTokensScored = StatisticsVariable.getStatisticsVariable(
                props.getContext(), "curTokensScored");
        tokensCreated =
                StatisticsVariable.getStatisticsVariable(props.getContext(),
                        "tokensCreated");

        showTokenCount = props.getBoolean(PROP_SHOW_TOKEN_COUNT, 
                                          PROP_SHOW_TOKEN_COUNT_DEFAULT);
        checkStateOrder = props.getBoolean(PROP_CHECK_STATE_ORDER,
                                           PROP_CHECK_STATE_ORDER_DEFAULT);
        buildWordLattice = props.getBoolean(PROP_BUILD_WORD_LATTICE,
                                            PROP_BUILD_WORD_LATTICE_DEFAULT);
    }


    /**
     * Called at the start of recognition. Gets the search manager
     * ready to recognize
     */
    public void start() {
        linguist.start();
        pruner.start();
        scorer.start();
        localStart();
    }

    /**
     * Performs the recognition for the given number of frames.
     *
     * @param nFrames the number of frames to recognize
     *
     * @return the current result
     */
    public Result recognize(int nFrames) {
        boolean done = false;
        Result result;

        for (int i = 0; i < nFrames && !done; i++) {
	    // System.out.println("Frame " + currentFrameNumber);
            // score the emitting list

            activeList = activeBucket.getEmittingList();
            if (activeList != null) {
                done = !scoreTokens();
                if (!done) {
                    bestTokenMap = new HashMap(activeList.size() * 5);
                    // prune and grow the emitting list
                    pruneBranches();
                    currentFrameNumber++;
                    resultList = new LinkedList();
                    growBranches();
                    // prune and grow the non-emitting lists
		    // activeBucket.dump();
                    growNonEmittingLists();
                }
            }
        }

        result = new Result(loserManager, activeList, resultList, 
                            currentFrameNumber, done);

        if (showTokenCount) {
            showTokenCount();
        }
        return result;
    }


    /**
     * Terminates a recognition
     */
    public void stop() {
        localStop();
        scorer.stop();
        pruner.stop();
        linguist.stop();
    }


    /**
     * Gets the initial grammar node from the linguist and
     * creates a GrammarNodeToken
     */
    protected void localStart() {
        currentFrameNumber = 0;
        curTokensScored.value = 0;

        try {
            stateOrder = linguist.getSearchStateOrder();
            activeBucket = new SimpleActiveListManager(props, stateOrder);
            if (buildWordLattice) {
                loserManager = new AlternateHypothesisManager(props);
            }

            SearchState state = linguist.getInitialSearchState();
            
            activeList = (ActiveList)
                Class.forName
                (props.getString(PROP_ACTIVE_LIST_TYPE,
                                 PROP_ACTIVE_LIST_TYPE_DEFAULT)).newInstance();
            activeList.setProperties(props);
            activeList.add(new Token(state, currentFrameNumber));
	    resultList = new LinkedList();

            bestTokenMap = new HashMap();
            growBranches();
            growNonEmittingLists();

        } catch (ClassNotFoundException fe) {
            throw new Error("Can't create active list", fe);
        } catch (InstantiationException ie) {
            throw new Error("Can't create active list", ie);
        } catch (IllegalAccessException iea) {
            throw new Error("Can't create active list", iea);
        }
    }


    /**
     * Local cleanup for this search manager
     */
    protected void localStop() {
    }


    /**
     * Goes through the active list of tokens and expands each
     * token, finding the set of successor tokens until all the successor
     * tokens are emitting tokens.
     *
     */
    protected void growBranches() {
        growTimer.start();
        Iterator iterator = activeList.iterator();
        float relativeBeamThreshold = activeList.getBeamThreshold();
	if (false) {
	   System.out.println("thresh : " + relativeBeamThreshold + " bs " +
			   activeList.getBestScore() + " tok " +
			   activeList.getBestToken());
	}
        while (iterator.hasNext()) {
            Token token = (Token) iterator.next();
            if (token.getScore() >= relativeBeamThreshold) {
                collectSuccessorTokens(token, relativeBeamThreshold);
            }
        }
        growTimer.stop();
    }


    /**
     * Grow the non-emitting ActiveLists, until the tokens reach
     * an emitting state.
     */
    private void growNonEmittingLists() {
        for (Iterator i = activeBucket.getNonEmittingListIterator();
             i.hasNext(); ) {
	    activeList = (ActiveList) i.next();
	    if (activeList != null) {
                i.remove();
		pruneBranches();
		growBranches();
	    }
        }
    }


    /**
     * Calculate the acoustic scores for the active list. The active
     * list should contain only emitting tokens.
     *
     * @return <code>true</code>  if there are more frames to score,
     * otherwise, false
     *
     */
    protected boolean scoreTokens() {
	boolean moreTokens;
        Token bestToken = null;
	scoreTimer.start();
        bestToken = (Token) scorer.calculateScores(activeList.getTokens());
	scoreTimer.stop();

        moreTokens =  (bestToken != null);
        activeList.setBestToken(bestToken);

        // monitorWords(activeList);
        // monitorStates(activeList);
        if (false) {
            System.out.println("BEST " + bestToken);
        }

	curTokensScored.value += activeList.size();
	totalTokensScored.value += activeList.size();

	return moreTokens;

    }


    /**
     * Keeps track of and reports all of the active word histories for
     * the given active list
     *
     * @param activeList the activelist to track
     */
    private void monitorWords(ActiveList activeList) {
        WordTracker tracker = new WordTracker(currentFrameNumber);

        for (Iterator i = activeList.iterator(); i.hasNext(); ) {
            Token t = (Token) i.next();
            tracker.add(t);
        }
        tracker.dump();
    }


    int tokenSum = 0;
    int count = 0;
    /**
     * Keeps track of and reports statistics about the number of
     * active states
     *
     * @param activeList the active list of states
     */
    private void monitorStates(ActiveList activeList) {

        tokenSum += activeList.size();
        count++;

        if ((count % 100) == 0) {
            System.out.println("Tokens: " + activeList.size() + 
                " avg " + (tokenSum / count));
        }
    }



    /**
     * Removes unpromising branches from the active list
     *
     */
    protected void pruneBranches() {
        pruneTimer.start();
        activeList =  pruner.prune(activeList);
        pruneTimer.stop();
    }

    /**
     * Gets the best token for this state
     *
     * @param state the state of interest
     *
     * @return the best token
     */
    protected Token getBestToken(SearchState state) {
        Object key = getStateKey(state);

        if (!wantTokenStacks) {
            return (Token) bestTokenMap.get(key);
        } else {
            // new way... if the heap for this state isn't full return 
            // null, otherwise return the worst scoring token
            TokenHeap th = (TokenHeap) bestTokenMap.get(key);
            Token t;

            if (th == null) {
                return null;
            } else if ((t = th.get(state)) != null) {
                return t;
            } else if (!th.isFull()) {
                return null;
            } else {
                return th.getSmallest();
            }
        }
    }

    /**
     * Sets the best token for a given state
     *
     * @param token the best token
     *
     * @param state the state
     *
     * @return the previous best token for the given state, or null if
     *    no previous best token
     */
    protected void setBestToken(Token token, SearchState state) {

        Object key = getStateKey(state);
        if (!wantTokenStacks) {
            bestTokenMap.put(key, token);
        } else {
            TokenHeap th = (TokenHeap) bestTokenMap.get(key);
            if (th == null) {
                th = new TokenHeap(maxTokenHeapSize);
                bestTokenMap.put(key, th);
            }
            th.add(token);
        }
    }

    protected Token getWordPredecessor(Token token) {
        if (true) {    // DEBUG: see all predecessors
            return token; 
        }
        while (token != null && !token.isWord()) {
            token = token.getPredecessor();
        }
        return token;
    }


    private Object getStateKey(SearchState state) {
        if (!wantTokenStacks) {
            return state;
        } else {
            if (state.isEmitting()) {
                return new  SinglePathThroughHMMKey(((HMMSearchState) state));
                // return ((HMMSearchState) state).getHMMState().getHMM();
            } else {
                 return state;
            }
        }
    }

    /**
     * Checks that the given two states are in legitimate order.
     */
    private void checkStateOrder(SearchState fromState, SearchState toState) {
        Class fromClass = fromState.getClass();
        Class toClass = toState.getClass();

        // first, find where in stateOrder is the fromState
        int i = 0;
        for (; i < stateOrder.length; i++) {
            if (stateOrder[i] == fromClass) {
                break;
            }
        }

        // We are assuming that the last state in the state order
        // is an emitting state. We assume that emitting states can
        // expand to any state type. So if (i == (stateOrder.length)),
        // which means that fromState is an emitting state, we don't
        // do any checks.

        if (i < (stateOrder.length - 1)) {
            for (int j = 0; j <= i; j++) {
                if (stateOrder[j] == toClass) {
                    throw new Error("IllegalState order: from " + 
                                    fromState.getClass().getName() + " " +
                                    fromState.toPrettyString() + " to " + 
                                    toState.getClass().getName() + " " +
                                    toState.toPrettyString());
                }
            }
        }
    }

    /**
     * Collects the next set of emitting tokens from a token
     * and accumulates them in the active or result lists
     *
     * @param token  the token to collect successors from
     * be immediately expaned are placed. Null if we should always
     * expand all nodes.
     *
     * @param threshold the minimum score the token must have in order
     *    for it to be grown
     */
    protected void collectSuccessorTokens(Token token, float threshold) {

        // If this is a final state, add it to the final list

        if (token.isFinal()) {
            resultList.add(getWordPredecessor(token));
            return;
        }

        if (token.getScore() < threshold) {
            return;
        }

        SearchState state = token.getSearchState();
        SearchStateArc[] arcs = state.getSuccessors();
        Token wordPredecessor = getWordPredecessor(token);
        
        // For each successor
        // calculate the entry score for the token based upon the
        // predecessor token score and the transition probabilities
        // if the score is better than the best score encountered for
        // the SearchState and frame then create a new token, add
        // it to the lattice and the SearchState.
        // If the token is an emitting token add it to the list,
        // othewise recursively collect the new tokens successors.

        for (int i = 0; i < arcs.length; i++) {
            SearchStateArc arc = arcs[i];
            SearchState nextState = arc.getState();

            if (checkStateOrder) {
                checkStateOrder(state, nextState);
            }

            // We're actually multiplying the variables, but since
            // these come in log(), multiply gets converted to add
            float logEntryScore = token.getScore() +  arc.getProbability();


            Token bestToken = getBestToken(nextState);
            boolean firstToken = bestToken == null ;

            if (firstToken || bestToken.getScore() < logEntryScore) {
                Token newBestToken = new Token(wordPredecessor,
                                               nextState,
                                               logEntryScore,
                                               arc.getLanguageProbability(),
                                               arc.getInsertionProbability(),
                                               currentFrameNumber);
                tokensCreated.value++;

                setBestToken(newBestToken, nextState);
                if (firstToken) {
		    activeBucket.add(newBestToken);
                } else {
                    if (false) {
                        System.out.println("Replacing " + bestToken + " with " 
                                + newBestToken);
                    }
		    activeBucket.replace(bestToken, newBestToken);
		    if (buildWordLattice && newBestToken.isWord()) {
                        
                        // Move predecessors of bestToken to precede 
                        // newBestToken, bestToken will be garbage collected.
                        loserManager.changeSuccessor(newBestToken,bestToken);
                        loserManager.addAlternatePredecessor
                            (newBestToken,bestToken.getPredecessor());
                    }
                }
            } else {
                if (buildWordLattice && 
                    nextState instanceof WordSearchState)  {
                    if (wordPredecessor != null) {
                        loserManager.addAlternatePredecessor
                            (bestToken, wordPredecessor);
                    }
                }
            }
        }
    }



    /**
     * Counts all the tokens in the active list (and displays them).
     * This is an expensive operation.
     */
    private void showTokenCount() {
        Set tokenSet = new HashSet();

        for (Iterator i = activeList.iterator(); i.hasNext(); ) {
            Token token = (Token) i.next();
            while (token != null) {
                tokenSet.add(token);
                token = token.getPredecessor();
            }
        }

        System.out.println("Token Lattice size: " + tokenSet.size());

        tokenSet = new HashSet();

        for (Iterator i = resultList.iterator(); i.hasNext(); ) {
            Token token = (Token) i.next();
            while (token != null) {
                tokenSet.add(token);
                token = token.getPredecessor();
            }
        }

        System.out.println("Result Lattice size: " + tokenSet.size());
    }


    /**
     * Returns the Linguist.
     *
     * @return the Linguist
     */
    public Linguist getLinguist() {
        return linguist;
    }


    /**
     * Returns the Pruner.
     *
     * @return the Pruner
     */
    public Pruner getPruner() {
        return pruner;
    }


    /**
     * Returns the AcousticScorer.
     *
     * @return the AcousticScorer
     */
    public AcousticScorer getAcousticScorer() {
        return scorer;
    }


    /**
     * Returns the LogMath used.
     *
     * @return the LogMath used
     */
    public LogMath getLogMath() {
        return logMath;
    }


    /**
     * Returns the SphinxProperties used.
     *
     * @return the SphinxProperties used
     */
    public SphinxProperties getSphinxProperties() {
        return props;
    }


    /**
     * Returns the best token map.
     *
     * @return the best token map
     */
    protected Map getBestTokenMap() {
        return bestTokenMap;
    }


    /**
     * Sets the best token Map.
     *
     * @param bestTokenMap the new best token Map
     */
    protected void setBestTokenMap(Map bestTokenMap) {
        this.bestTokenMap = bestTokenMap;
    }


    /**
     * Returns the ActiveList.
     *
     * @return the ActiveList
     */
    public ActiveList getActiveList() {
        return activeList;
    }


    /**
     * Sets the ActiveList.
     *
     * @param activeList the new ActiveList
     */
    public void setActiveList(ActiveList activeList) {
        this.activeList = activeList;
    }


    /**
     * Returns the result list.
     *
     * @return the result list
     */
    public List getResultList() {
        return resultList;
    }


    /**
     * Sets the result list.
     *
     * @param resultList the new result list
     */
    public void setResultList(List resultList) {
        this.resultList = resultList;
    }


    /**
     * Returns the current frame number.
     *
     * @return the current frame number
     */
    public int getCurrentFrameNumber() {
        return currentFrameNumber;
    }


    /**
     * Returns the Timer for growing.
     *
     * @return the Timer for growing
     */
    public Timer getGrowTimer() {
        return growTimer;
    }


    /**
     * Returns the tokensCreated StatisticsVariable.
     *
     * @return the tokensCreated StatisticsVariable.
     */
    public StatisticsVariable getTokensCreated() {
        return tokensCreated;
    }
}

/**
 * A 'best token' key. This key will allow hmm states that have
 * identical word histories and are in the same HMM state to be
 * treated equivalently. When used as the best token key, only the
 * best scoring token with a given word history survives per HMM.
 */
class SinglePathThroughHMMKey  {
    private HMMSearchState hmmSearchState;

    public SinglePathThroughHMMKey(HMMSearchState hmmSearchState) {
        this.hmmSearchState = hmmSearchState;
    }

    public int hashCode() {
        return hmmSearchState.getLexState().hashCode() * 13 +
               hmmSearchState.getWordHistory().hashCode();
    }

    public boolean equals(Object o) {
        if (this == o) {
            return true;
        } else if (o instanceof SinglePathThroughHMMKey) {
            SinglePathThroughHMMKey other = (SinglePathThroughHMMKey) o;
            boolean equal = hmmSearchState.getLexState().equals(
                    other.hmmSearchState.getLexState()) &&
                hmmSearchState.getWordHistory().equals(
                        other.hmmSearchState.getWordHistory());
            if (equal && false) {
                System.out.println("SPTHK A: " + hmmSearchState);
                System.out.println("SPTHK B: " + other.hmmSearchState);
            }
            return equal;
        }
        return false;
    }
}


/**
 * A quick and dirty token heap that allows us to perform token stack
 * experiments. It is not very efficient. We will likely replace this
 * with something better once we figure out how we want to prune
 * things.
 */
class TokenHeap {
    Token[] tokens;
    int curSize = 0;

    /**
     * Creates a token heap with the maximum size
     *
     * @param maxSize the maximum size of the heap
     */
    TokenHeap(int maxSize) {
        tokens = new Token[maxSize];
    }

    /**
     * Adds a token to the heap
     *
     * @param token the token to add
     */
    void add(Token token) {
        // first, if an identical state exists, replace
        // it.

        if (!tryReplace(token)) {
            if (curSize < tokens.length) {
                tokens[curSize++] = token;
            } else if (token.getScore() > tokens[curSize - 1].getScore()) {
                tokens[curSize - 1] = token;
            }
        }
        fixupInsert();
    }

    /**
     * Returns the smallest scoring token on the heap
     *
     * @return the smallest scoring token
     */
    Token getSmallest() {
        if (curSize == 0) {
            return null;
        } else {
            return tokens[curSize - 1];
        }
    }

    /**
     * Determines if the heap is ful
     *
     * @return <code>true</code> if the heap is full
     */
    boolean isFull() {
        return curSize == tokens.length;
    }

    /**
     * Checks to see if there is already a token t on the heap that
     * has the same search state. If so, this token replaces that one
     *
     * @param t the token to try to add to the heap
     *
     * @return <code>true</code> if the token was added
     */
    private boolean tryReplace(Token t) {
        for (int i = 0; i < curSize; i++) {
            if (t.getSearchState().equals(tokens[i].getSearchState())) {
                assert t.getScore() > tokens[i].getScore();
                tokens[i] = t;
                return true;
            }
        }
        return false;
    }


    /**
     * Orders the heap after an insert
     */
    private void fixupInsert() {
        Arrays.sort(tokens, 0, curSize - 1, Token.COMPARATOR);
    }

    /**
     * returns a token on the heap that matches the given search state
     *
     * @param s the search state
     *
     * @return the token that matches, or null
     */
    Token  get(SearchState s) {
        for (int i = 0; i < curSize; i++) {
            if (tokens[i].getSearchState().equals(s)) {
                return tokens[i];
            }
        }
        return null;
    }
}


/**
 * A class that keeps track of word histories 
 */
class WordTracker {
    Map statMap;
    int frameNumber;
    int stateCount;
    int maxWordHistories;

    /**
     * Creates a word tracker for the given frame number
     *
     * @param frameNumber the frame number
     */
    WordTracker(int frameNumber) {
        statMap = new HashMap();
        this.frameNumber = frameNumber;
    }

    /**
     * Adds a word history for the given token to the word tracker
     *
     * @param t the token to add
     */
    void add(Token t) {
        stateCount++;
        WordSequence ws = getWordSequence(t);
        WordStats stats = (WordStats) statMap.get(ws);
        if (stats == null) {
            stats = new WordStats(ws);
            statMap.put(ws, stats);
        }
        stats.update(t);
    }

    /**
     * Dumps the word histories in the tracker
     */
    void dump() {
        dumpSummary();
        Object[] stats = statMap.values().toArray();
        Arrays.sort(stats, WordStats.COMPARATOR);
        for (int i = 0; i < stats.length; i++) {
            System.out.println("   " + stats[i]);
        }
    }

    void prune() {
        Object[] stats = statMap.values().toArray();
        Arrays.sort(stats, WordStats.COMPARATOR);
        for (int i = 0; i < maxWordHistories; i++) {
            System.out.println("   " + stats[i]);
        }
    }

    /**
     * Dumps summary information in the tracker
     */
    void dumpSummary() {
        System.out.println("Frame: " + frameNumber 
                + " states: " + stateCount + " histories " + statMap.size());
    }

    /**
     * Given a token, gets the word sequence represented by the token
     *
     * @param token the token of interest
     *
     * @return the word sequence for the token
     */
    private WordSequence getWordSequence(Token token) {
        List wordList = new LinkedList();

        while (token != null) {
            if (token.isWord()) {
                WordSearchState wordState =
                        (WordSearchState) token.getSearchState();
                Word word = wordState.getPronunciation().getWord();
                wordList.add(0, word);
            }
            token = token.getPredecessor();
        }
        return WordSequence.getWordSequence(wordList);
    }
}

/**
 * Keeps track of statistics for a particular word sequence
 */
class WordStats {
    public final static Comparator COMPARATOR = new Comparator() {
	    public int compare(Object o1, Object o2) {
		WordStats ws1 = (WordStats) o1;
		WordStats ws2 = (WordStats) o2;

		if (ws1.maxScore > ws2.maxScore) {
		    return -1;
		} else if (ws1.maxScore ==  ws2.maxScore) {
		    return 0;
		} else {
		    return 1;
		}
	    }
	};

    int size;
    float maxScore;
    float minScore;
    WordSequence ws;

    /**
     * Creates a word stat for the given sequence
     *
     * @param ws the word sequence
     */
    WordStats(WordSequence ws) {
        size = 0;
        maxScore = -Float.MAX_VALUE;
        minScore = Float.MAX_VALUE;
        this.ws = ws;
    }

    /**
     * Updates the stats based upon the scores for the given token
     *
     * @param t the token
     */
    void update(Token t) {
        size++;
        if (t.getScore() > maxScore) {
            maxScore = t.getScore();
        }
        if (t.getScore() < minScore) {
            minScore = t.getScore();
        }
    }

    /**
     * Returns a string representation of the stats
     *
     * @return a string representation
     */
    public String toString() {
        return "states:" + size + " max:" + maxScore + " min:" + minScore +
            " " + ws;
    }
}
