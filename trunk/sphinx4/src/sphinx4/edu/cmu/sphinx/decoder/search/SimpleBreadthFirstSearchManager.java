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
package edu.cmu.sphinx.decoder.search;

import edu.cmu.sphinx.decoder.pruner.Pruner;
import edu.cmu.sphinx.decoder.scorer.AcousticScorer;
import edu.cmu.sphinx.linguist.Linguist;
import edu.cmu.sphinx.linguist.SearchState;
import edu.cmu.sphinx.linguist.SearchStateArc;
import edu.cmu.sphinx.linguist.WordSearchState;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.StatisticsVariable;
import edu.cmu.sphinx.util.Timer;
import edu.cmu.sphinx.util.props.*;

import java.io.IOException;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Provides the breadth first search. To perform recognition an application should call initialize before recognition
 * begins, and repeatedly call <code> recognize </code> until Result.isFinal() returns true. Once a final result has
 * been obtained, <code> terminate </code> should be called.
 * <p/>
 * <p/>
 * All scores and probabilities are maintained in the log math log domain.
 * <p/>
 * For information about breadth first search please refer to "Spoken Language Processing", X. Huang, PTR
 */

// TODO - need to add in timing code.
public class SimpleBreadthFirstSearchManager implements SearchManager {

    /** Sphinx property that defines the name of the linguist to be used by this search manager. */
    @S4Component(type = Linguist.class)
    public final static String PROP_LINGUIST = "linguist";

    /** Sphinx property that defines the name of the linguist to be used by this search manager. */
    @S4Component(type = Pruner.class)
    public final static String PROP_PRUNER = "pruner";

    /** Sphinx property that defines the name of the scorer to be used by this search manager. */
    @S4Component(type = AcousticScorer.class)
    public final static String PROP_SCORER = "scorer";

    /** Sphinx property that defines the name of the logmath to be used by this search manager. */
    @S4Component(type = LogMath.class)
    public final static String PROP_LOG_MATH = "logMath";

    /** Sphinx property that defines the name of the active list factory to be used by this search manager. */
    @S4Component(type = ActiveListFactory.class)
    public final static String PROP_ACTIVE_LIST_FACTORY = "activeListFactory";

    /**
     * A sphinx property than, when set to <code>true</code> will cause the recognizer to count up all the tokens in the
     * active list after every frame.
     */
    @S4Boolean(defaultValue = false)
    public final static String PROP_SHOW_TOKEN_COUNT = "showTokenCount";

    /**
     * Property that sets the minimum score relative to the maximum score in the word list for pruning. Words with a
     * score less than relativeBeamWidth * maximumScore will be pruned from the list
     */
    @S4Double(defaultValue = 0.0)
    public final static String PROP_RELATIVE_WORD_BEAM_WIDTH = "relativeWordBeamWidth";

    /**
     * A sphinx property that controls whether or not relative beam pruning will be performed on the entry into a
     * state.
     */
    @S4Boolean(defaultValue = false)
    public final static String PROP_WANT_ENTRY_PRUNING = "wantEntryPruning";

    /**
     * A sphinx property that controls the number of frames processed for every time the decode growth step is skipped.
     * Setting this property to zero disables grow skipping. Setting this number to a small integer will increase the
     * speed of the decoder but will also decrease its accuracy. The higher the number, the less often the grow code is
     * skipped.
     */
    @S4Integer(defaultValue = 0)
    public final static String PROP_GROW_SKIP_INTERVAL = "growSkipInterval";


    private Linguist linguist; // Provides grammar/language info
    private Pruner pruner; // used to prune the active list
    private AcousticScorer scorer; // used to score the active list
    private int currentFrameNumber; // the current frame number
    private ActiveList activeList; // the list of active tokens
    private List<Token> resultList; // the current set of results
    private LogMath logMath;
    private Logger logger;

    // ------------------------------------
    // monitoring data
    // ------------------------------------
    private Timer scoreTimer; // TODO move these timers out
    private Timer pruneTimer;
    private Timer growTimer;
    private StatisticsVariable totalTokensScored;
    private StatisticsVariable tokensPerSecond;
    private StatisticsVariable curTokensScored;
    private StatisticsVariable tokensCreated;
    private StatisticsVariable viterbiPruned;
    private StatisticsVariable beamPruned;

    private boolean showTokenCount;
    private boolean wantEntryPruning;
    private Map<SearchState, Token> bestTokenMap;
    private float logRelativeWordBeamWidth;
    private int totalHmms;
    private double startTime = 0;
    private float threshold;
    private float wordThreshold;
    private int growSkipInterval = 0;
    private ActiveListFactory activeListFactory;


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util.props.PropertySheet)
    */
    public void newProperties(PropertySheet ps) throws PropertyException {
        logger = ps.getLogger();
        logMath = (LogMath) ps.getComponent(PROP_LOG_MATH);

        linguist = (Linguist) ps.getComponent(PROP_LINGUIST);
        pruner = (Pruner) ps.getComponent(PROP_PRUNER);
        scorer = (AcousticScorer) ps.getComponent(PROP_SCORER);
        activeListFactory = (ActiveListFactory) ps.getComponent(PROP_ACTIVE_LIST_FACTORY);
        showTokenCount = ps.getBoolean(PROP_SHOW_TOKEN_COUNT);

        double relativeWordBeamWidth = ps.getDouble(PROP_RELATIVE_WORD_BEAM_WIDTH);
        growSkipInterval = ps.getInt(PROP_GROW_SKIP_INTERVAL);
        wantEntryPruning = ps.getBoolean(PROP_WANT_ENTRY_PRUNING);
        logRelativeWordBeamWidth = logMath.linearToLog(relativeWordBeamWidth);
    }


    /** Called at the start of recognition. Gets the search manager ready to recognize */
    public void startRecognition() {
        linguist.startRecognition();
        pruner.startRecognition();
        scorer.startRecognition();
        localStart();
        if (startTime == 0.0) {
            startTime = System.currentTimeMillis();
        }
    }


    /**
     * Performs the recognition for the given number of frames.
     *
     * @param nFrames the number of frames to recognize
     * @return the current result or null if there is no Result (due to the lack of frames to recognize)
     */
    public Result recognize(int nFrames) {
        boolean done = false;
        boolean noData = false;
        Result result = null;
        for (int i = 0; i < nFrames && !done; i++) {
            done = recognize();
            if (done && currentFrameNumber <= 0) {
                noData = true;
            }
        }
        if (!noData) {
            result = new Result(activeList, resultList, currentFrameNumber,
                    done, logMath);
        }
        if (showTokenCount) {
            showTokenCount();
        }
        return result;
    }


    /** Terminates a recognition */
    public void stopRecognition() {
        localStop();
        scorer.stopRecognition();
        pruner.stopRecognition();
        linguist.stopRecognition();
    }


    /**
     * Performs recognition for one frame. Returns true if recognition has been completed.
     *
     * @return <code>true</code> if recognition is completed.
     */
    protected boolean recognize() {
        boolean more = scoreTokens(); // score emitting tokens
        if (more) {
            pruneBranches(); // eliminate poor branches
            currentFrameNumber++;
            if (growSkipInterval == 0
                    || (currentFrameNumber % growSkipInterval) != 0) {
                growBranches(); // extend remaining branches
            }
        }
        return !more;
    }


    /** Gets the initial grammar node from the linguist and creates a GrammarNodeToken */
    protected void localStart() {
        currentFrameNumber = 0;
        curTokensScored.value = 0;
        ActiveList newActiveList = activeListFactory.newInstance();
        SearchState state = linguist.getSearchGraph().getInitialState();
        newActiveList.add(new Token(state, currentFrameNumber));
        activeList = newActiveList;

        growBranches();
    }


    /** Local cleanup for this search manager */
    protected void localStop() {
    }


    /**
     * Goes through the active list of tokens and expands each token, finding the set of successor tokens until all the
     * successor tokens are emitting tokens.
     */
    protected void growBranches() {
        int mapSize = activeList.size() * 10;
        if (mapSize == 0) {
            mapSize = 1;
        }
        growTimer.start();
        bestTokenMap = new HashMap<SearchState, Token>(mapSize);
        ActiveList oldActiveList = activeList;
        Iterator oldListIterator = activeList.iterator();
        resultList = new LinkedList<Token>();
        activeList = activeListFactory.newInstance();
        threshold = oldActiveList.getBeamThreshold();
        wordThreshold = oldActiveList.getBestScore() + logRelativeWordBeamWidth;

        while (oldListIterator.hasNext()) {
            Token token = (Token) oldListIterator.next();
            collectSuccessorTokens(token);
        }
        growTimer.stop();
        if (logger.isLoggable(Level.FINE)) {
            int hmms = activeList.size();
            totalHmms += hmms;
            logger.fine("Frame: " + currentFrameNumber + " Hmms: "
                    + hmms + "  total " + totalHmms);
        }
    }


    /**
     * Calculate the acoustic scores for the active list. The active list should contain only emitting tokens.
     *
     * @return <code>true</code> if there are more frames to score, otherwise, false
     */
    protected boolean scoreTokens() {
        boolean moreTokens;
        Token bestToken = null;
        scoreTimer.start();
        bestToken = (Token) scorer.calculateScores(activeList.getTokens());
        scoreTimer.stop();
        moreTokens = (bestToken != null);
        activeList.setBestToken(bestToken);
        curTokensScored.value += activeList.size();
        totalTokensScored.value += activeList.size();
        tokensPerSecond.value = totalTokensScored.value / getTotalTime();
        if (logger.isLoggable(Level.FINE)) {
            logger.fine(currentFrameNumber + " " + activeList.size()
                    + " " + curTokensScored.value + " "
                    + (int) tokensPerSecond.value);
        }
        return moreTokens;
    }


    /**
     * Returns the total time since we start4ed
     *
     * @return the total time (in seconds)
     */
    private double getTotalTime() {
        return (System.currentTimeMillis() - startTime) / 1000.0;
    }


    /** Removes unpromising branches from the active list */
    protected void pruneBranches() {
        int startSize = activeList.size();
        pruneTimer.start();
        activeList = pruner.prune(activeList);
        beamPruned.value += startSize - activeList.size();
        pruneTimer.stop();
    }


    /**
     * Gets the best token for this state
     *
     * @param state the state of interest
     * @return the best token
     */
    protected Token getBestToken(SearchState state) {
        Token best = bestTokenMap.get(state);
        if (logger.isLoggable(Level.FINER) && best != null) {
            logger.finer("BT " + best + " for state " + state);
        }
        return best;
    }


    /**
     * Sets the best token for a given state
     *
     * @param token the best token
     * @param state the state
     * @return the previous best token for the given state, or null if no previous best token
     */
    protected Token setBestToken(Token token, SearchState state) {
        return bestTokenMap.put(state, token);
    }


    /**
     * Collects the next set of emitting tokens from a token and accumulates them in the active or result lists
     *
     * @param token the token to collect successors from
     */
    protected void collectSuccessorTokens(Token token) {
        SearchState state = token.getSearchState();
        // If this is a final state, add it to the final list
        if (token.isFinal()) {
            resultList.add(token);
        }
        if (token.getScore() < threshold) {
            return;
        }
        if (state instanceof WordSearchState
                && token.getScore() < wordThreshold) {
            return;
        }
        SearchStateArc[] arcs = state.getSuccessors();
        // For each successor
        // calculate the entry score for the token based upon the
        // predecessor token score and the transition probabilities
        // if the score is better than the best score encountered for
        // the SearchState and frame then create a new token, add
        // it to the lattice and the SearchState.
        // If the token is an emitting token add it to the list,
        // othewise recursively collect the new tokens successors.
        for (SearchStateArc arc : arcs) {
            SearchState nextState = arc.getState();
            // We're actually multiplying the variables, but since
            // these come in log(), multiply gets converted to add
            float logEntryScore = token.getScore() + arc.getProbability();
            if (wantEntryPruning) {
                if (logEntryScore < threshold) {
                    continue;
                }
                if (nextState instanceof WordSearchState
                        && logEntryScore < wordThreshold) {
                    continue;
                }
            }
            Token bestToken = getBestToken(nextState);
            boolean firstToken = bestToken == null;
            if (firstToken || bestToken.getScore() <= logEntryScore) {
                Token newToken = token.child(nextState, logEntryScore, arc
                        .getLanguageProbability(), arc
                        .getInsertionProbability(), currentFrameNumber);
                tokensCreated.value++;
                setBestToken(newToken, nextState);
                if (!newToken.isEmitting()) {
                    // if not emitting, check to see if we've already visited
                    // this state during this frame. Expand the token only if we
                    // haven't visited it already. This prevents the search
                    // from getting stuck in a loop of states with no
                    // intervening emitting nodes. This can happen with nasty
                    // jsgf grammars such as ((foo*)*)*
                    if (!isVisited(newToken)) {
                        collectSuccessorTokens(newToken);
                    }
                } else {
                    if (firstToken) {
                        activeList.add(newToken);
                    } else {
                        activeList.replace(bestToken, newToken);
                        viterbiPruned.value++;
                    }
                }
            } else {
                viterbiPruned.value++;
            }
        }
    }


    /**
     * Determines whether or not we've visited the state associated with this token since the previous frame.
     *
     * @param t the token to check
     * @return true if we've visted the search state since the last frame
     */
    private boolean isVisited(Token t) {
        SearchState curState = t.getSearchState();

        t = t.getPredecessor();

        while (t != null && !t.isEmitting()) {
            if (curState.equals(t.getSearchState())) {
                return true;
            }
            t = t.getPredecessor();
        }
        return false;
    }


    /** Counts all the tokens in the active list (and displays them). This is an expensive operation. */
    private void showTokenCount() {
        if (logger.isLoggable(Level.INFO)) {
            Set<Token> tokenSet = new HashSet<Token>();
            for (Iterator i = activeList.iterator(); i.hasNext();) {
                Token token = (Token) i.next();
                while (token != null) {
                    tokenSet.add(token);
                    token = token.getPredecessor();
                }
            }
            logger.info("Token Lattice size: " + tokenSet.size());
            tokenSet = new HashSet<Token>();
            for (Token token : resultList) {
                while (token != null) {
                    tokenSet.add(token);
                    token = token.getPredecessor();
                }
            }
            logger.info("Result Lattice size: " + tokenSet.size());
        }
    }


    /**
     * Returns the best token map.
     *
     * @return the best token map
     */
    protected Map<SearchState, Token> getBestTokenMap() {
        return bestTokenMap;
    }


    /**
     * Sets the best token Map.
     *
     * @param bestTokenMap the new best token Map
     */
    protected void setBestTokenMap(Map<SearchState, Token> bestTokenMap) {
        this.bestTokenMap = bestTokenMap;
    }


    /**
     * Returns the result list.
     *
     * @return the result list
     */
    public List<Token> getResultList() {
        return resultList;
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


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.decoder.search.SearchManager#allocate()
    */
    public void allocate() throws IOException {
        totalTokensScored = StatisticsVariable
                .getStatisticsVariable("totalTokensScored");
        tokensPerSecond = StatisticsVariable
                .getStatisticsVariable("tokensScoredPerSecond");
        curTokensScored = StatisticsVariable
                .getStatisticsVariable("curTokensScored");
        tokensCreated = StatisticsVariable
                .getStatisticsVariable("tokensCreated");
        viterbiPruned = StatisticsVariable
                .getStatisticsVariable("viterbiPruned");
        beamPruned = StatisticsVariable.getStatisticsVariable("beamPruned");
        linguist.allocate();
        pruner.allocate();
        scorer.allocate();

        scoreTimer = Timer.getTimer("scoring");
        pruneTimer = Timer.getTimer("pruning");
        growTimer = Timer.getTimer("growing");
    }


    /*
    * (non-Javadoc)
    *
    * @see edu.cmu.sphinx.decoder.search.SearchManager#deallocate()
    */
    public void deallocate() {
        scorer.deallocate();
        pruner.deallocate();
        linguist.deallocate();
    }


    public Linguist getLinguist() {
        return linguist;
    }


    public Pruner getPruner() {
        return pruner;
    }


    public AcousticScorer getScorer() {
        return scorer;
    }
}
