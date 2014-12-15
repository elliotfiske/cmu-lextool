package edu.cmu.sphinx.linguist.allphone;

import java.io.IOException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

import edu.cmu.sphinx.linguist.Linguist;
import edu.cmu.sphinx.linguist.SearchGraph;
import edu.cmu.sphinx.linguist.acoustic.AcousticModel;
import edu.cmu.sphinx.linguist.acoustic.HMM;
import edu.cmu.sphinx.linguist.acoustic.LeftRightContext;
import edu.cmu.sphinx.linguist.acoustic.UnitManager;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.S4Boolean;
import edu.cmu.sphinx.util.props.S4Component;
import edu.cmu.sphinx.util.props.S4Double;

public class AllphoneLinguist implements Linguist {

    /** The property that defines the acoustic model to use when building the search graph */
    @S4Component(type = AcousticModel.class)
    public final static String PROP_ACOUSTIC_MODEL = "acousticModel";
    
    /**
     * The property that controls phone insertion probability.
     * Default value for context independent phoneme decoding is 0.05,
     * while for context dependent - 0.01.
     */
    @S4Double(defaultValue = 0.05)
    public final static String PROP_PIP = "phoneInsertionProbability";
    
    /**
     * The property that controls whether to use context dependent phones.
     * Changing it for true, don't forget to tune phone insertion probability.
     */
    @S4Boolean(defaultValue = false)
    public final static String PROP_CD = "useContextDependentPhones";
    
    private AcousticModel acousticModel;
    private ArrayList<HMM> ciHMMs;
    private ArrayList<HMM> fillerHMMs;
    private ArrayList<HMM> leftContextSilHMMs;
    private HashMap<Integer, HashMap<Integer, ArrayList<HMM>>> cdHMMs;
    private float pip;
    private boolean useCD;
    
    public AllphoneLinguist() {    
        
    }
    
    public void newProperties(PropertySheet ps) throws PropertyException {
        acousticModel = (AcousticModel) ps.getComponent(PROP_ACOUSTIC_MODEL);
        pip = LogMath.getLogMath().linearToLog(ps.getFloat(PROP_PIP));
        
        useCD = ps.getBoolean(PROP_CD);
        if (useCD)
            createContextDependentSuccessors();
        else
            createContextIndependentSuccessors();
    }

    public SearchGraph getSearchGraph() {
        return new AllphoneSearchGraph(this);
    }

    public void startRecognition() {
    }

    public void stopRecognition() {
    }

    public void allocate() throws IOException {
    }

    public void deallocate() throws IOException {
    }
    
    public AcousticModel getAcousticModel() {
        return acousticModel;
    }
    
    public float getPhoneInsertionProb() {
        return pip;
    }
    
    public boolean useContextDependentPhones() {
        return useCD;
    }
    
    public ArrayList<HMM> getCISuccessors() {
        return ciHMMs;
    }
    
    public ArrayList<HMM> getCDSuccessors(int lc, int base) {
        if (lc == UnitManager.SILENCE.getBaseID())
            return leftContextSilHMMs;
        if (base == UnitManager.SILENCE.getBaseID())
            return fillerHMMs;
        return cdHMMs.get(lc).get(base);
    }

    private void createContextIndependentSuccessors() {
        Iterator<HMM> hmmIter = acousticModel.getHMMIterator();
        ciHMMs = new ArrayList<HMM>();
        while (hmmIter.hasNext()) {
            HMM hmm = hmmIter.next();
            if (!hmm.getUnit().isContextDependent())
                ciHMMs.add(hmm);
        }
    }
    
    private void createContextDependentSuccessors() {
        cdHMMs = new HashMap<Integer, HashMap<Integer, ArrayList<HMM>>>();
        fillerHMMs = new ArrayList<HMM>();
        leftContextSilHMMs = new ArrayList<HMM>();
        Iterator<HMM> hmmIter = acousticModel.getHMMIterator();
        while (hmmIter.hasNext()) {
            HMM hmm = hmmIter.next();
            if (hmm.getUnit().isFiller()) {
                fillerHMMs.add(hmm);
                continue;
            }
            if (hmm.getUnit().isContextDependent()) {
                LeftRightContext context = (LeftRightContext)hmm.getUnit().getContext();
                int lc = context.getLeftContext()[0].getBaseID();
                if (lc == UnitManager.SILENCE.getBaseID()) {
                    leftContextSilHMMs.add(hmm);
                    continue;
                }
                int baseId = hmm.getUnit().getBaseID();
                HashMap<Integer, ArrayList<HMM>> lcSuccessors; 
                if ((lcSuccessors = cdHMMs.get(lc)) == null) {
                    lcSuccessors = new HashMap<Integer, ArrayList<HMM>>();
                    cdHMMs.put(lc, lcSuccessors);
                }
                ArrayList<HMM> lcBaseSuccessors;
                if ((lcBaseSuccessors = lcSuccessors.get(baseId)) == null) {
                    lcBaseSuccessors = new ArrayList<HMM>();
                    lcSuccessors.put(baseId, lcBaseSuccessors);
                }
                lcBaseSuccessors.add(hmm);
            }
        }
        leftContextSilHMMs.addAll(fillerHMMs);
    }
    
}
