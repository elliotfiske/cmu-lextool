package edu.cmu.sphinx.linguist.allphone;

import java.io.IOException;

import edu.cmu.sphinx.linguist.Linguist;
import edu.cmu.sphinx.linguist.SearchGraph;
import edu.cmu.sphinx.linguist.acoustic.AcousticModel;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.S4Component;
import edu.cmu.sphinx.util.props.S4Double;

public class AllphoneLinguist implements Linguist {

    /** The property that defines the acoustic model to use when building the search graph */
    @S4Component(type = AcousticModel.class)
    public final static String PROP_ACOUSTIC_MODEL = "acousticModel";
    
    /**
     * The property that controls size of lookahead window.
     * Acceptable values are in range [1..10].
     */
    @S4Double(defaultValue = 0.05)
    public final static String PROP_PIP = "phoneInsertionProbability";
    
    private AcousticModel acousticModel;
    private float pip;
    
    public AllphoneLinguist() {    
        
    }
    
    public void newProperties(PropertySheet ps) throws PropertyException {
        acousticModel = (AcousticModel) ps.getComponent(PROP_ACOUSTIC_MODEL);
        pip = LogMath.getLogMath().linearToLog(ps.getFloat(PROP_PIP));
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

}
