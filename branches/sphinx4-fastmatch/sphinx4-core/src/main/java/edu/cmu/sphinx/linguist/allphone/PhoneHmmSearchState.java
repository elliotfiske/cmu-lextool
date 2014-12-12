package edu.cmu.sphinx.linguist.allphone;

import edu.cmu.sphinx.decoder.scorer.ScoreProvider;
import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.linguist.SearchState;
import edu.cmu.sphinx.linguist.SearchStateArc;
import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.acoustic.HMMState;
import edu.cmu.sphinx.linguist.acoustic.HMMStateArc;
import edu.cmu.sphinx.linguist.acoustic.Unit;

public class PhoneHmmSearchState implements SearchState, SearchStateArc, ScoreProvider {

    private Unit unit;
    private HMMState state;
    private AllphoneLinguist linguist;
    
    private float insertionProb;
    private float languageProb;
    
    public PhoneHmmSearchState(Unit unit, HMMState hmmState, AllphoneLinguist linguist, float insertionProb, float languageProb) {
        this.unit = unit;
        this.state = hmmState;
        this.linguist = linguist;
        this.insertionProb = insertionProb;
        this.languageProb = languageProb;
    }

    public SearchState getState() {
        return this;
    }
    
    public int getBaseId() {
    	return unit.getBaseID();
    }

    public float getProbability() {
        return getLanguageProbability() + getInsertionProbability();
    }

    public float getLanguageProbability() {
        return languageProb;
    }

    public float getInsertionProbability() {
        return insertionProb;
    }

    /* If we are final, transfer to all possible phones, otherwise
     * return all successors of this hmm state.
     * */
    public SearchStateArc[] getSuccessors() {
        if (state.isExitState()) {
            SearchStateArc[] result = new SearchStateArc[1];
            result[0] = new PhoneNonEmittingSearchState(unit, linguist, insertionProb, languageProb);
            return result;
        } else {
            HMMStateArc successors[] = state.getSuccessors();
            SearchStateArc[] results = new SearchStateArc[successors.length];
            for (int i = 0; i < successors.length; i++) {
                results[i] = new PhoneHmmSearchState(unit, successors[i].getHMMState(), linguist, insertionProb, languageProb);
            }
            return results;
        }
    }

    public boolean isEmitting() {
        return state.isEmitting();
    }

    public boolean isFinal() {
        return false;
    }

    public String toPrettyString() {
        return "HMM " + state.toString();
    }

    public String getSignature() {
        return null;
    }

    public WordSequence getWordHistory() {
        return null;
    }

    public Object getLexState() {
        return null;
    }

    public int getOrder() {
        return 1;
    }

    @Override
    public float getScore(Data data) {
        return state.getScore(data);
    }

    @Override
    public float[] getComponentScore(Data feature) {
        return state.calculateComponentScore(feature);
    }

    @Override
    public boolean equals(Object obj) {
        if (!(obj instanceof PhoneHmmSearchState))
            return false;
        boolean haveSameBaseId = ((PhoneHmmSearchState)obj).unit.getBaseID() == unit.getBaseID();
        boolean haveSameHmmState = ((PhoneHmmSearchState)obj).state.getState() == state.getState();
        return haveSameBaseId && haveSameHmmState;
    }

    @Override
    public int hashCode() {
        return unit.getBaseID() * 37 + state.getState();
    }
}
