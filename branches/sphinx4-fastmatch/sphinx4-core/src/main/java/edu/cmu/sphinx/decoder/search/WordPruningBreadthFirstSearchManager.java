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

// a test search manager.

import edu.cmu.sphinx.decoder.pruner.Pruner;
import edu.cmu.sphinx.decoder.scorer.AcousticScorer;
import edu.cmu.sphinx.decoder.scorer.QueuedAcousticScorer;
import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.linguist.*;
import edu.cmu.sphinx.linguist.allphone.PhoneHmmSearchState;
import edu.cmu.sphinx.linguist.lextree.LexTreeLinguist.LexTreeHMMState;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.StatisticsVariable;
import edu.cmu.sphinx.util.Timer;
import edu.cmu.sphinx.util.TimerPool;
import edu.cmu.sphinx.util.props.*;

import java.io.IOException;
import java.util.*;
import java.util.logging.Level;
import java.util.logging.Logger;

/**
 * Provides the breadth first search. To perform recognition an application
 * should call initialize before recognition begins, and repeatedly call
 * <code> recognize </code> until Result.isFinal() returns true. Once a final
 * result has been obtained, <code> stopRecognition </code> should be called.
 * <p/>
 * All scores and probabilities are maintained in the log math log domain.
 */

public class WordPruningBreadthFirstSearchManager extends TokenSearchManager {

    /**
     * The property that defines the name of the linguist to be used by this
     * search manager.
     */
    @S4Component(type = Linguist.class)
    public final static String PROP_LINGUIST = "linguist";
    
    /** The property that defines the name of the linguist to be used for fast match. */
    @S4Component(mandatory = false, type = Linguist.class)
    public final static String PROP_FASTMATCH_LINGUIST = "fastmatchLinguist";

    /**
     * The property that defines the name of the linguist to be used by this
     * search manager.
     */
    @S4Component(type = Pruner.class)
    public final static String PROP_PRUNER = "pruner";

    /**
     * The property that defines the name of the scorer to be used by this
     * search manager.
     */
    @S4Component(type = AcousticScorer.class)
    public final static String PROP_SCORER = "scorer";

    /**
     * The property than, when set to <code>true</code> will cause the
     * recognizer to count up all the tokens in the active list after every
     * frame.
     */
    @S4Boolean(defaultValue = false)
    public final static String PROP_SHOW_TOKEN_COUNT = "showTokenCount";

    /**
     * The property that controls the number of frames processed for every time
     * the decode growth step is skipped. Setting this property to zero disables
     * grow skipping. Setting this number to a small integer will increase the
     * speed of the decoder but will also decrease its accuracy. The higher the
     * number, the less often the grow code is skipped. Values like 6-8 is known
     * to be the good enough for large vocabulary tasks. That means that one of
     * 6 frames will be skipped.
     */
    @S4Integer(defaultValue = 0)
    public final static String PROP_GROW_SKIP_INTERVAL = "growSkipInterval";

    /** The property that defines the type of active list to use */
    @S4Component(type = ActiveListManager.class)
    public final static String PROP_ACTIVE_LIST_MANAGER = "activeListManager";
    
    /** The property that defines the type active list factory for fast match */
    @S4Component(mandatory = false, type = ActiveListFactory.class)
    public final static String PROP_FM_ACTIVE_LIST_FACTORY = "fastmatchActiveListFactory";

    /** The property for checking if the order of states is valid. */
    @S4Boolean(defaultValue = false)
    public final static String PROP_CHECK_STATE_ORDER = "checkStateOrder";

    /** The property that specifies the maximum lattice edges */
    @S4Integer(defaultValue = 100)
    public final static String PROP_MAX_LATTICE_EDGES = "maxLatticeEdges";

    /**
     * The property that controls the amount of simple acoustic lookahead
     * performed. Setting the property to zero (the default) disables simple
     * acoustic lookahead. The lookahead need not be an integer.
     */
    @S4Double(defaultValue = 0)
    public final static String PROP_ACOUSTIC_LOOKAHEAD_FRAMES = "acousticLookaheadFrames";
    
    @S4Double(defaultValue = 1.0)
    public final static String PROP_LOOKAHEAD_PENALTY_WEIGHT = "lookaheadPenaltyWeight";

    /** The property that specifies the relative beam width */
    @S4Double(defaultValue = 0.0)
    // TODO: this should be a more meaningful default e.g. the common 1E-80
    public final static String PROP_RELATIVE_BEAM_WIDTH = "relativeBeamWidth";
    
    /**
     * The property that control whether to use lookahead heuristic
     */
    @S4Boolean(defaultValue = false)
    public final static String PROP_DO_LOOKAHEAD = "doLookahead";

    /**
     * The property that controls size of lookahead window.
     * Acceptable values are in range [1..10].
     */
    @S4Integer(defaultValue = 1)
    public final static String PROP_LOOKAHEAD_WINDOW = "lookaheadWindow";

    // -----------------------------------
    // Configured Subcomponents
    // -----------------------------------
    private Linguist linguist; // Provides grammar/language info
    protected Linguist fastmatchLinguist; // Provides phones info for fastmatch
    private Pruner pruner; // used to prune the active list
    private AcousticScorer scorer; // used to score the active list
    private ActiveListManager activeListManager;
    private ActiveListFactory fastmatchActiveListFactory;
    private LogMath logMath;

    // -----------------------------------
    // Configuration data
    // -----------------------------------
    private Logger logger;
    private boolean showTokenCount;
    private boolean checkStateOrder;
    private int growSkipInterval;
    private float relativeBeamWidth;
    private float acousticLookaheadFrames;
    private int maxLatticeEdges = 100;

    // -----------------------------------
    // Instrumentation
    // -----------------------------------
    private Timer scoreTimer;
    private Timer pruneTimer;
    private Timer growTimer;
    private StatisticsVariable totalTokensScored;
    private StatisticsVariable curTokensScored;
    private StatisticsVariable tokensCreated;
    private long tokenSum;
    private int tokenCount;

    // -----------------------------------
    // Lookahead data
    // -----------------------------------
    private boolean doLookahead;
    private int lookaheadWindow;
    private float lookaheadWeight;
    private HashMap<Integer, Float> penalties;
    private LinkedList<FrameCiScores> ciScores;
    
    // -----------------------------------
    // Working data
    // -----------------------------------
    private int currentFrameNumber; // the current frame number
    private int currentFastMatchFrameNumber; // the current frame number for lookahead matching
    protected ActiveList activeList; // the list of active tokens
    protected ActiveList fastmatchActiveList; // the list of active tokens for fast match
    private List<Token> resultList; // the current set of results
    protected Map<SearchState, Token> bestTokenMap;
    protected Map<SearchState, Token> fastMatchBestTokenMap;
    private AlternateHypothesisManager loserManager;
    private int numStateOrder;
    // private TokenTracker tokenTracker;
    // private TokenTypeTracker tokenTypeTracker;
    private boolean delayStreamEnd;
    private boolean streamPause;

    /**
     * 
     * @param linguist
     * @param pruner
     * @param scorer
     * @param activeListManager
     * @param showTokenCount
     * @param relativeWordBeamWidth
     * @param growSkipInterval
     * @param checkStateOrder
     * @param buildWordLattice
     * @param maxLatticeEdges
     * @param acousticLookaheadFrames
     * @param keepAllTokens
     */
    public WordPruningBreadthFirstSearchManager(Linguist linguist, Linguist fastmatchLinguist, Pruner pruner, AcousticScorer scorer,
            ActiveListManager activeListManager, ActiveListFactory fastmatchActiveListFactory, boolean showTokenCount, double relativeWordBeamWidth, int growSkipInterval,
            boolean checkStateOrder, boolean buildWordLattice, boolean doLookahead, int lookaheadWindow, float lookaheadWeight, int maxLatticeEdges, float acousticLookaheadFrames,
            boolean keepAllTokens) {

        this.logger = Logger.getLogger(getClass().getName());
        this.logMath = LogMath.getLogMath();
        this.linguist = linguist;
        this.fastmatchLinguist = fastmatchLinguist;
        this.pruner = pruner;
        this.scorer = scorer;
        this.activeListManager = activeListManager;
        this.fastmatchActiveListFactory = fastmatchActiveListFactory;
        this.showTokenCount = showTokenCount;
        this.growSkipInterval = growSkipInterval;
        this.checkStateOrder = checkStateOrder;
        this.buildWordLattice = buildWordLattice;
        this.doLookahead = doLookahead;
        this.lookaheadWindow = lookaheadWindow;
        if (doLookahead && (lookaheadWindow < 1 || lookaheadWindow > 10))
        	throw new IllegalArgumentException("Unsupported lookahead window size: " + lookaheadWindow + ". Value in range [1..10] is expected");
        if (doLookahead && !(scorer instanceof QueuedAcousticScorer))
        	throw new IllegalArgumentException("To do lookahead, AcousticScorer should be instance of QueuedAcousticScorer");
        this.lookaheadWeight = lookaheadWeight;
        this.ciScores = new LinkedList<FrameCiScores>();
        this.penalties = new HashMap<Integer, Float>();
        this.maxLatticeEdges = maxLatticeEdges;
        this.acousticLookaheadFrames = acousticLookaheadFrames;
        this.keepAllTokens = keepAllTokens;

        this.relativeBeamWidth = logMath.linearToLog(relativeWordBeamWidth);
    }

    public WordPruningBreadthFirstSearchManager() {

    }

    /*
     * (non-Javadoc)
     * 
     * @see
     * edu.cmu.sphinx.util.props.Configurable#newProperties(edu.cmu.sphinx.util
     * .props.PropertySheet)
     */
    @Override
    public void newProperties(PropertySheet ps) throws PropertyException {
        super.newProperties(ps);

        logMath = LogMath.getLogMath();
        logger = ps.getLogger();

        linguist = (Linguist) ps.getComponent(PROP_LINGUIST);
        fastmatchLinguist = (Linguist) ps.getComponent(PROP_FASTMATCH_LINGUIST);
        pruner = (Pruner) ps.getComponent(PROP_PRUNER);
        scorer = (AcousticScorer) ps.getComponent(PROP_SCORER);
        activeListManager = (ActiveListManager) ps.getComponent(PROP_ACTIVE_LIST_MANAGER);
        fastmatchActiveListFactory = (ActiveListFactory) ps.getComponent(PROP_FM_ACTIVE_LIST_FACTORY);
        showTokenCount = ps.getBoolean(PROP_SHOW_TOKEN_COUNT);
        growSkipInterval = ps.getInt(PROP_GROW_SKIP_INTERVAL);
        doLookahead = ps.getBoolean(PROP_DO_LOOKAHEAD);
        lookaheadWindow = ps.getInt(PROP_LOOKAHEAD_WINDOW);
        if (doLookahead && (lookaheadWindow < 1 || lookaheadWindow > 10))
        	throw new PropertyException(WordPruningBreadthFirstSearchManager.class.getName(), PROP_LOOKAHEAD_WINDOW, 
        			"Unsupported lookahead window size: " + lookaheadWindow + ". Value in range [1..10] is expected");
        if (doLookahead && !(scorer instanceof QueuedAcousticScorer))
        	throw new PropertyException(WordPruningBreadthFirstSearchManager.class.getName(), PROP_SCORER,
        			"To do lookahead, AcousticScorer should be instance of QueuedAcousticScorer");
        lookaheadWeight = ps.getFloat(PROP_LOOKAHEAD_PENALTY_WEIGHT);
        ciScores = new LinkedList<FrameCiScores>();
        penalties = new HashMap<Integer, Float>();
        checkStateOrder = ps.getBoolean(PROP_CHECK_STATE_ORDER);
        maxLatticeEdges = ps.getInt(PROP_MAX_LATTICE_EDGES);
        acousticLookaheadFrames = ps.getFloat(PROP_ACOUSTIC_LOOKAHEAD_FRAMES);

        relativeBeamWidth = logMath.linearToLog(ps.getDouble(PROP_RELATIVE_BEAM_WIDTH));
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.decoder.search.SearchManager#allocate()
     */
    public void allocate() {
        // tokenTracker = new TokenTracker();
        // tokenTypeTracker = new TokenTypeTracker();

        scoreTimer = TimerPool.getTimer(this, "Score");
        pruneTimer = TimerPool.getTimer(this, "Prune");
        growTimer = TimerPool.getTimer(this, "Grow");

        totalTokensScored = StatisticsVariable.getStatisticsVariable("totalTokensScored");
        curTokensScored = StatisticsVariable.getStatisticsVariable("curTokensScored");
        tokensCreated = StatisticsVariable.getStatisticsVariable("tokensCreated");

        try {
            linguist.allocate();
            pruner.allocate();
            scorer.allocate();
        } catch (IOException e) {
            throw new RuntimeException("Allocation of search manager resources failed", e);
        }
    }

    /*
     * (non-Javadoc)
     * 
     * @see edu.cmu.sphinx.decoder.search.SearchManager#deallocate()
     */
    public void deallocate() {
        try {
            scorer.deallocate();
            pruner.deallocate();
            linguist.deallocate();
        } catch (IOException e) {
            throw new RuntimeException("Deallocation of search manager resources failed", e);
        }
    }

    /**
     * Called at the start of recognition. Gets the search manager ready to
     * recognize
     */
    public void startRecognition() {
        linguist.startRecognition();
        pruner.startRecognition();
        scorer.startRecognition();
        localStart();
    }

    /**
     * Performs the recognition for the given number of frames.
     * 
     * @param nFrames
     *            the number of frames to recognize
     * @return the current result
     */
    public Result recognize(int nFrames) {
        boolean done = false;
        Result result = null;
        delayStreamEnd = false;

        for (int i = 0; i < nFrames && !done; i++) {
            if (doLookahead && !streamPause)
            	fastMatchRecognize();
        	done = recognize();
        }

        if (!delayStreamEnd) {
            result = new Result(loserManager, activeList, resultList, currentFrameNumber, done, linguist.getSearchGraph()
                    .getWordTokenFirst());
        }

        // tokenTypeTracker.show();
        if (showTokenCount) {
            showTokenCount();
        }
        return result;
    }
    
    private void fastMatchRecognize() {
    	boolean more = scoreFastMatchTokens();
    	
    	if (more) {
    		pruneFastMatchBranches();
    		currentFastMatchFrameNumber++;
    		createFastMatchBestTokenMap();
    		growFastmatchBranches();
    	}
    }
    
    private boolean recognize() {

        activeList = activeListManager.getEmittingList();
        boolean more = scoreTokens();

        if (more) {
            pruneBranches();
            currentFrameNumber++;
            if (growSkipInterval == 0 || (currentFrameNumber % growSkipInterval) != 0) {
                clearCollectors();
                growEmittingBranches();
                growNonEmittingBranches();
            }
        }
        return !more;
    }

    /**
     * Clears lists and maps before next expansion stage
     */
    private void clearCollectors() {
        resultList = new LinkedList<Token>();
        createBestTokenMap();
        activeListManager.clearEmittingList();
    }

    /**
     * creates a new best token map with the best size
     */
    protected void createFastMatchBestTokenMap() {
        int mapSize = fastmatchActiveList.size() * 10;
        if (mapSize == 0) {
            mapSize = 1;
        }
        fastMatchBestTokenMap = new HashMap<SearchState, Token>(mapSize);
    }    
    
    /**
     * creates a new best token map with the best size
     */
    protected void createBestTokenMap() {
        int mapSize = activeList.size() * 10;
        if (mapSize == 0) {
            mapSize = 1;
        }
        bestTokenMap = new HashMap<SearchState, Token>(mapSize, 0.3F);
    }

    /** Terminates a recognition */
    public void stopRecognition() {
        localStop();
        scorer.stopRecognition();
        pruner.stopRecognition();
        linguist.stopRecognition();
    }

    /**
     * Gets the initial grammar node from the linguist and creates a
     * GrammarNodeToken
     */
    protected void localStart() {
        SearchGraph searchGraph = linguist.getSearchGraph();
        currentFrameNumber = 0;
        currentFastMatchFrameNumber = 0;
        curTokensScored.value = 0;
        numStateOrder = searchGraph.getNumStateOrder();
        activeListManager.setNumStateOrder(numStateOrder);
        if (buildWordLattice) {
            loserManager = new AlternateHypothesisManager(maxLatticeEdges);
        }

        if (doLookahead) {
        	//prepare fast match active list
        	fastmatchActiveList = fastmatchActiveListFactory.newInstance();
            SearchState fmState = fastmatchLinguist.getSearchGraph().getInitialState();
            fastmatchActiveList.add(new Token(fmState, currentFastMatchFrameNumber));
            createFastMatchBestTokenMap();
            growFastmatchBranches();
            streamPause = false;
            for (int i = 0; (i < lookaheadWindow) && !streamPause; i++)
            	fastMatchRecognize();
        }
        
        SearchState state = searchGraph.getInitialState();

        activeList = activeListManager.getEmittingList();
        activeList.add(new Token(state, currentFrameNumber));
        
        clearCollectors();

        growBranches();
        growNonEmittingBranches();
        // tokenTracker.setEnabled(false);
        // tokenTracker.startUtterance();
    }

    /** Local cleanup for this search manager */
    protected void localStop() {
        // tokenTracker.stopUtterance();
    }
    
    /**
     * Goes through the fast match active list of tokens and expands each token, 
     * finding the set of successor tokens until all the successor tokens are emitting tokens.
     */
    protected void growFastmatchBranches() {
        growTimer.start();
        ActiveList oldActiveList = fastmatchActiveList;
        fastmatchActiveList = fastmatchActiveListFactory.newInstance();
        float fastmathThreshold = oldActiveList.getBeamThreshold();
        float[] frameCiScores = new float[100];
        Arrays.fill(frameCiScores, -Float.MAX_VALUE);
        float frameMaxCiScore = -Float.MAX_VALUE;
        for (Token token : oldActiveList) {
        	float tokenScore = token.getScore();
        	if (tokenScore < fastmathThreshold)
        		continue;
        	//TODO do we need phone insertion probability, i.e. threshold WordSearchStates?
            //filling max ci scores array that will be used in general search token score composing
        	if (token.getSearchState() instanceof PhoneHmmSearchState) {
            	int baseId = ((PhoneHmmSearchState)token.getSearchState()).getBaseId();
            	if (frameCiScores[baseId] < tokenScore)
            		frameCiScores[baseId] = tokenScore;
            	if (frameMaxCiScore < tokenScore)
            		frameMaxCiScore = tokenScore;
            }
        	collectFastMatchSuccessorTokens(token);
        }
        ciScores.add(new FrameCiScores(frameCiScores, frameMaxCiScore));
        penalties.clear();
        if (ciScores.size() > lookaheadWindow)
        	ciScores.poll();
        growTimer.stop();
    }
    
    /**
     * Goes through the active list of tokens and expands each token, finding
     * the set of successor tokens until all the successor tokens are emitting
     * tokens.
     */
    protected void growBranches() {
        growTimer.start();
        float relativeBeamThreshold = activeList.getBeamThreshold();
        if (logger.isLoggable(Level.FINE)) {
            logger.fine("Frame: " + currentFrameNumber + " thresh : " + relativeBeamThreshold + " bs "
                    + activeList.getBestScore() + " tok " + activeList.getBestToken());
        }
        for (Token token : activeList) {
        	Float penalty = 0.0f;
        	if (doLookahead && token.getSearchState() instanceof LexTreeHMMState) {
        		int baseId = ((LexTreeHMMState)token.getSearchState()).getHMMState().getHMM().getBaseUnit().getBaseID();
        		if ((penalty = penalties.get(baseId)) == null)
        			penalty = updateLookaheadPenalty(baseId);
        	}
            if (token.getScore() + (penalty * lookaheadWeight) >= relativeBeamThreshold && allowExpansion(token)) {
                collectSuccessorTokens(token);
            }
        }
        growTimer.stop();
    }

    /**
     * Grows the emitting branches. This version applies a simple acoustic
     * lookahead based upon the rate of change in the current acoustic score.
     */
    protected void growEmittingBranches() {
        if (acousticLookaheadFrames > 0F) {
            growTimer.start();
            float bestScore = -Float.MAX_VALUE;
            for (Token t : activeList) {
                float score = t.getScore() + t.getAcousticScore() * acousticLookaheadFrames;
                if (score > bestScore) {
                    bestScore = score;
                }
            }
            float relativeBeamThreshold = bestScore + relativeBeamWidth;
            for (Token t : activeList) {
            	Float penalty = 0.0f;
            	if (doLookahead && t.getSearchState() instanceof LexTreeHMMState) {
            		int baseId = ((LexTreeHMMState)t.getSearchState()).getHMMState().getHMM().getBaseUnit().getBaseID();
            		if ((penalty = penalties.get(baseId)) == null)
            			penalty = updateLookaheadPenalty(baseId);
            	}
            	if (t.getScore() + t.getAcousticScore() * acousticLookaheadFrames + (penalty * lookaheadWeight) > relativeBeamThreshold)
                    collectSuccessorTokens(t);
            }
            growTimer.stop();
        } else {
            growBranches();
        }
    }

    /**
     * Grow the non-emitting branches, until the tokens reach an emitting state.
     */
    private void growNonEmittingBranches() {
        for (Iterator<ActiveList> i = activeListManager.getNonEmittingListIterator(); i.hasNext();) {
            activeList = i.next();
            if (activeList != null) {
                i.remove();
                pruneBranches();
                growBranches();
            }
        }
    }

    protected boolean scoreFastMatchTokens() {
    	boolean moreTokens;
        scoreTimer.start();
        Data data = ((QueuedAcousticScorer)scorer).calculateScores(fastmatchActiveList.getTokens(), true);
        scoreTimer.stop();
        
        Token bestToken = null;
        if (data instanceof Token) {
            bestToken = (Token) data;
        } else {
            streamPause = true;
        }

        moreTokens = (bestToken != null);
        fastmatchActiveList.setBestToken(bestToken);
        
        // monitorWords(activeList);
        monitorStates(fastmatchActiveList);

        // System.out.println("BEST " + bestToken);

        curTokensScored.value += fastmatchActiveList.size();
        totalTokensScored.value += fastmatchActiveList.size();
    	
        return moreTokens;
    }
    
    /**
     * Calculate the acoustic scores for the active list. The active list should
     * contain only emitting tokens.
     * 
     * @return <code>true</code> if there are more frames to score, otherwise,
     *         false
     */
    protected boolean scoreTokens() {
        boolean moreTokens;
        scoreTimer.start();
        Data data = scorer.calculateScores(activeList.getTokens());
        scoreTimer.stop();

        Token bestToken = null;
        if (data instanceof Token) {
            bestToken = (Token) data;
        } else if (data == null) {
            delayStreamEnd = true;
        }

        moreTokens = (bestToken != null);
        activeList.setBestToken(bestToken);

        // monitorWords(activeList);
        monitorStates(activeList);

        // System.out.println("BEST " + bestToken);

        curTokensScored.value += activeList.size();
        totalTokensScored.value += activeList.size();

        return moreTokens;
    }

    /**
     * Keeps track of and reports all of the active word histories for the given
     * active list
     * 
     * @param activeList
     *            the active list to track
     */
    @SuppressWarnings("unused")
    private void monitorWords(ActiveList activeList) {

        // WordTracker tracker1 = new WordTracker(currentFrameNumber);
        //
        // for (Token t : activeList) {
        // tracker1.add(t);
        // }
        // tracker1.dump();
        //
        // TokenTracker tracker2 = new TokenTracker();
        //
        // for (Token t : activeList) {
        // tracker2.add(t);
        // }
        // tracker2.dumpSummary();
        // tracker2.dumpDetails();
        //
        // TokenTypeTracker tracker3 = new TokenTypeTracker();
        //
        // for (Token t : activeList) {
        // tracker3.add(t);
        // }
        // tracker3.dump();

        // StateHistoryTracker tracker4 = new
        // StateHistoryTracker(currentFrameNumber);

        // for (Token t : activeList) {
        // tracker4.add(t);
        // }
        // tracker4.dump();
    }

    /**
     * Keeps track of and reports statistics about the number of active states
     * 
     * @param activeList
     *            the active list of states
     */
    private void monitorStates(ActiveList activeList) {

        tokenSum += activeList.size();
        tokenCount++;

        if ((tokenCount % 1000) == 0) {
            logger.info("Average Tokens/State: " + (tokenSum / tokenCount));
        }
    }

    /** Removes unpromising branches from the fast match active list */
    protected void pruneFastMatchBranches() {
        pruneTimer.start();
        fastmatchActiveList = pruner.prune(fastmatchActiveList);
        pruneTimer.stop();
    }
    
    /** Removes unpromising branches from the active list */
    protected void pruneBranches() {
        pruneTimer.start();
        activeList = pruner.prune(activeList);
        pruneTimer.stop();
    }
    
    protected Token getFastMatchBestToken(SearchState state) {
    	return fastMatchBestTokenMap.get(state);
    }
    
    /**
     * Gets the best token for this state
     * 
     * @param state
     *            the state of interest
     * @return the best token
     */
    protected Token getBestToken(SearchState state) {
        return bestTokenMap.get(state);
    }

    protected void setFastMatchBestToken(Token token, SearchState state) {
    	fastMatchBestTokenMap.put(state, token);
    }
    
    /**
     * Sets the best token for a given state
     * 
     * @param token
     *            the best token
     * @param state
     *            the state
     */
    protected void setBestToken(Token token, SearchState state) {
        bestTokenMap.put(state, token);
    }

    /**
     * Checks that the given two states are in legitimate order.
     * 
     * @param fromState
     * @param toState
     */
    private void checkStateOrder(SearchState fromState, SearchState toState) {
        if (fromState.getOrder() == numStateOrder - 1) {
            return;
        }

        if (fromState.getOrder() > toState.getOrder()) {
            throw new Error("IllegalState order: from " + fromState.getClass().getName() + ' ' + fromState.toPrettyString()
                    + " order: " + fromState.getOrder() + " to " + toState.getClass().getName() + ' ' + toState.toPrettyString()
                    + " order: " + toState.getOrder());
        }
    }
    
    protected void collectFastMatchSuccessorTokens(Token token) {
        SearchState state = token.getSearchState();
        SearchStateArc[] arcs = state.getSuccessors();
        // For each successor
        // calculate the entry score for the token based upon the
        // predecessor token score and the transition probabilities
        // if the score is better than the best score encountered for
        // the SearchState and frame then create a new token, add
        // it to the lattice and the SearchState.
        // If the token is an emitting token add it to the list,
        // otherwise recursively collect the new tokens successors.
        for (SearchStateArc arc : arcs) {
            SearchState nextState = arc.getState();
            // We're actually multiplying the variables, but since
            // these come in log(), multiply gets converted to add
            float logEntryScore = token.getScore() + arc.getProbability();
            Token predecessor = getResultListPredecessor(token);
            
            // if not emitting, check to see if we've already visited
            // this state during this frame. Expand the token only if we
            // haven't visited it already. This prevents the search
            // from getting stuck in a loop of states with no
            // intervening emitting nodes. This can happen with nasty
            // jsgf grammars such as ((foo*)*)*
            if (!nextState.isEmitting()) {
                Token newToken = new Token(predecessor, nextState, logEntryScore,
                        arc.getInsertionProbability(),
                        arc.getLanguageProbability(), 
                        currentFastMatchFrameNumber);
                tokensCreated.value++;
                if (!isVisited(newToken)) {
                	collectFastMatchSuccessorTokens(newToken);
                }
                continue;
            }
            
            Token bestToken = getFastMatchBestToken(nextState);
            if (bestToken == null) {        
                Token newToken = new Token(predecessor, nextState, logEntryScore,
                        arc.getInsertionProbability(),
                        arc.getLanguageProbability(), 
                        currentFastMatchFrameNumber);
                tokensCreated.value++;
                setFastMatchBestToken(newToken, nextState);
                fastmatchActiveList.add(newToken);
            } else {
                if (bestToken.getScore() <= logEntryScore) {
                    bestToken.update(predecessor, nextState, logEntryScore,
                            arc.getInsertionProbability(),
                            arc.getLanguageProbability(), 
                            currentFastMatchFrameNumber);
                }
            }
        }
    }
    
    /**
     * Collects the next set of emitting tokens from a token and accumulates
     * them in the active or result lists
     * 
     * @param token
     *            the token to collect successors from be immediately expanded
     *            are placed. Null if we should always expand all nodes.
     */
    protected void collectSuccessorTokens(Token token) {

        // tokenTracker.add(token);
        // tokenTypeTracker.add(token);

        // If this is a final state, add it to the final list

        if (token.isFinal()) {
            resultList.add(getResultListPredecessor(token));
            return;
        }

        // if this is a non-emitting token and we've already
        // visited the same state during this frame, then we
        // are in a grammar loop, so we don't continue to expand.
        // This check only works properly if we have kept all of the
        // tokens (instead of skipping the non-word tokens).
        // Note that certain linguists will never generate grammar loops
        // (lextree linguist for example). For these cases, it is perfectly
        // fine to disable this check by setting keepAllTokens to false

        if (!token.isEmitting() && (keepAllTokens && isVisited(token))) {
            return;
        }

        SearchState state = token.getSearchState();
        SearchStateArc[] arcs = state.getSuccessors();
        Token predecessor = getResultListPredecessor(token);

        // For each successor
        // calculate the entry score for the token based upon the
        // predecessor token score and the transition probabilities
        // if the score is better than the best score encountered for
        // the SearchState and frame then create a new token, add
        // it to the lattice and the SearchState.
        // If the token is an emitting token add it to the list,
        // otherwise recursively collect the new tokens successors.

        for (SearchStateArc arc : arcs) {
            SearchState nextState = arc.getState();

            if (checkStateOrder) {
                checkStateOrder(state, nextState);
            }

            // We're actually multiplying the variables, but since
            // these come in log(), multiply gets converted to add
            float logEntryScore = token.getScore() + arc.getProbability();

            Token bestToken = getBestToken(nextState);

            if (bestToken == null) {
                Token newBestToken = new Token(predecessor, nextState, logEntryScore, arc.getInsertionProbability(),
                        arc.getLanguageProbability(), currentFrameNumber);
                tokensCreated.value++;
                setBestToken(newBestToken, nextState);
                activeListAdd(newBestToken);
            } else if (bestToken.getScore() < logEntryScore) {
                // System.out.println("Updating " + bestToken + " with " +
                // newBestToken);
                Token oldPredecessor = bestToken.getPredecessor();
                bestToken.update(predecessor, nextState, logEntryScore, arc.getInsertionProbability(),
                        arc.getLanguageProbability(), currentFrameNumber);
                if (buildWordLattice && nextState instanceof WordSearchState) {
                    loserManager.addAlternatePredecessor(bestToken, oldPredecessor);
                }
            } else if (buildWordLattice && nextState instanceof WordSearchState) {
                if (predecessor != null) {
                    loserManager.addAlternatePredecessor(bestToken, predecessor);
                }
            }
        }
    }
    
    private Float updateLookaheadPenalty(int baseId) {
    	Float penalty = -Float.MAX_VALUE;
    	for (FrameCiScores frameCiScores : ciScores) {
    		float diff = frameCiScores.scores[baseId] - frameCiScores.maxScore;
    		if (diff > penalty)
    			penalty = diff;
    	}
    	penalties.put(baseId, penalty);
    	return penalty;
    }
    
    /**
     * Determines whether or not we've visited the state associated with this
     * token since the previous frame.
     * 
     * @param t
     * @return true if we've visited the search state since the last frame
     */
    private boolean isVisited(Token t) {
        SearchState curState = t.getSearchState();

        t = t.getPredecessor();

        while (t != null && !t.isEmitting()) {
            if (curState.equals(t.getSearchState())) {
                System.out.println("CS " + curState + " match " + t.getSearchState());
                return true;
            }
            t = t.getPredecessor();
        }
        return false;
    }

    protected void activeListAdd(Token token) {
        activeListManager.add(token);
    }

    /**
     * Determine if the given token should be expanded
     * 
     * @param t
     *            the token to test
     * @return <code>true</code> if the token should be expanded
     */
    protected boolean allowExpansion(Token t) {
        return true; // currently disabled
    }

    /**
     * Counts all the tokens in the active list (and displays them). This is an
     * expensive operation.
     */
    private void showTokenCount() {
        Set<Token> tokenSet = new HashSet<Token>();

        for (Token token : activeList) {
            while (token != null) {
                tokenSet.add(token);
                token = token.getPredecessor();
            }
        }

        System.out.println("Token Lattice size: " + tokenSet.size());

        tokenSet = new HashSet<Token>();

        for (Token token : resultList) {
            while (token != null) {
                tokenSet.add(token);
                token = token.getPredecessor();
            }
        }

        System.out.println("Result Lattice size: " + tokenSet.size());
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
     * @param activeList
     *            the new ActiveList
     */
    public void setActiveList(ActiveList activeList) {
        this.activeList = activeList;
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
     * Sets the result list.
     * 
     * @param resultList
     *            the new result list
     */
    public void setResultList(List<Token> resultList) {
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
    
    private class FrameCiScores {
    	public final float[] scores;
    	public final float maxScore;
    	
    	public FrameCiScores(float[] scores, float maxScore) {
    		this.scores = scores;
    		this.maxScore = maxScore;
    	}
    }

}
