package edu.cmu.sphinx.result;

import java.util.List;

import edu.cmu.sphinx.decoder.search.ActiveList;
import edu.cmu.sphinx.decoder.search.Token;
import edu.cmu.sphinx.frontend.FloatData;
import edu.cmu.sphinx.linguist.SearchState;
import edu.cmu.sphinx.linguist.UnitSearchState;
import edu.cmu.sphinx.linguist.acoustic.Unit;
import edu.cmu.sphinx.util.LogMath;

public class AlignerResult {

	private Result result;
	public AlignerResult(Result result) {
		this.result = result;
	}
	public String getBestTimedPhoneResult() {
		Token tok = result.getBestActiveToken();
		
		StringBuilder sb = new StringBuilder();
		float sampleRate = 0.0f;
		long lastSampleNumber = -1;
		long currSampleNumber = 0;
		boolean unitDetected = false;
		Unit lastUnit = null;
		while (tok != null) {
			FloatData data = (FloatData) tok.getData();
            SearchState searchState = tok.getSearchState();
            if(data != null){            	
            	currSampleNumber = data.getFirstSampleNumber();
            	if(lastSampleNumber == -1) {
            		lastSampleNumber = currSampleNumber;
            	}
            	if(sampleRate == 0.0f){
            		sampleRate = data.getSampleRate();           		
            	}
            }
            if(unitDetected) {
            	if(!lastUnit.isSilence()) {
            		sb.insert(0, lastUnit.getName() + "(" + (float)currSampleNumber/sampleRate +
            				"," + (float)lastSampleNumber/sampleRate + ") ");
            	}
            	lastSampleNumber = currSampleNumber;
            	unitDetected = false;
            }
            if (searchState instanceof UnitSearchState) {            	
                UnitSearchState unitState = (UnitSearchState) searchState;
                Unit unit = unitState.getUnit();
                lastUnit =unit;
                unitDetected = true;                
            }           
            tok = tok.getPredecessor();
        }
		return sb.toString();
	}

}
