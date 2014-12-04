/*
* Copyright 2014 Carnegie Mellon University.
* All Rights Reserved.  Use is subject to license terms.
*
* See the file "license.terms" for information on usage and
* redistribution of this file, and for a DISCLAIMER OF ALL
* WARRANTIES.
*
*/

package edu.cmu.sphinx.decoder.scorer;

import java.util.LinkedList;
import java.util.List;

import edu.cmu.sphinx.decoder.search.Token;
import edu.cmu.sphinx.frontend.BaseDataProcessor;
import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.frontend.DataEndSignal;
import edu.cmu.sphinx.frontend.DoubleData;
import edu.cmu.sphinx.frontend.Signal;
import edu.cmu.sphinx.frontend.endpoint.SpeechEndSignal;
import edu.cmu.sphinx.frontend.util.DataUtil;

/**
 * Extends SimpleAcousticScorer by providing queue scoring functionality
 * i.e. score acoustic data from front end twice: before putting it in queue
 * and after polling, so the one can do some preprocessing.
 * For example in case of lookahead heuristic.
 */
public class QueuedAcousticScorer extends SimpleAcousticScorer {
	
	private LinkedList<Data> dataQueue;
	
    public QueuedAcousticScorer(BaseDataProcessor frontEnd, ScoreNormalizer scoreNormalizer) {
		super(frontEnd, scoreNormalizer);
		dataQueue = new LinkedList<Data>();
    }

    public QueuedAcousticScorer() {
    	super();
    	dataQueue = new LinkedList<Data>();
    }
    
    @Override
    public void startRecognition() {
    	super.startRecognition();
    	dataQueue.clear();
    }
    
    /**
     * Scores the given set of states with fresh data or queued previously
     * depending on flag
     *
     * @param scoreableList A list containing scoreable objects to be scored
     * @param useNextData Flag that specifies whether to use fresh audio data for scoring or the one from queue
     * @return The best scoring scoreable, or <code>null</code> if there are no more features to score
     */
    public Data calculateScores(List<? extends Scoreable> scoreableList, boolean useNextData) {
    	try {
            Data data;
            if (useNextData) {
	            while ((data = getNextData()) instanceof Signal)
	                if (data instanceof SpeechEndSignal || data instanceof DataEndSignal)
	                	break;
	            if (data == null)
	            	return null;
	            dataQueue.add(data);
            } else {
            	if (dataQueue.isEmpty())
            		return null;
            	data = dataQueue.poll();
            }
            
            if (data instanceof SpeechEndSignal || data instanceof DataEndSignal) {
            	return data;
            }
            
            if (scoreableList.isEmpty())
            	return null;
            
            // convert the data to FloatData if not yet done
            if (data instanceof DoubleData)
                data = DataUtil.DoubleData2FloatData((DoubleData) data);

            Scoreable bestToken = doScoring(scoreableList, data);

            // apply optional score normalization
            if (scoreNormalizer != null && bestToken instanceof Token)
                bestToken = scoreNormalizer.normalize(scoreableList, bestToken);

            return bestToken;
        } catch (Exception e) {
            e.printStackTrace();
            return null;
        }
    }
    
    /**
     * Super call is used to compute scores for delayed data
     */
    @Override
    public Data calculateScores(List<? extends Scoreable> scoreableList) {
    	return calculateScores(scoreableList, false);
    }
	
}
