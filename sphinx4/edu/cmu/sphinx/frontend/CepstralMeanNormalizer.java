/**
 * [[[copyright]]]
 */

package edu.cmu.sphinx.frontend;

import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.Timer;

import java.io.IOException;


/**
 * Apply Cepstral Mean Normalization (CMN) to the set of input MFC frames,
 * that is, a CepstrumFrame. It subtracts the mean of all the input so far
 * from each cepstrum. The Sphinx properties that affect this processor
 * are: <pre>
 * edu.cmu.sphinx.frontend.cmn.initialCepstralMean
 * edu.cmu.sphinx.frontend.cmn.windowSize
 * edu.cmu.sphinx.frontend.cmn.shiftWindow </pre>
 * The mean of all the input cepstrum so far is not recalculated
 * for each cepstrum. This mean is recalculated after
 * <code>edu.cmu.sphinx.frontend.cmn.shiftWindow</code> cepstra.
 * This mean is calculated by dividing the sum of all input cepstrum so
 * far by the number of input cepstrum. After obtaining the mean,
 * the sum is exponentially by multiplying it by the ratio: <pre>
 * cmnWindow/(cmnWindow + number of frames since the last recalculation)</pre>
 *
 * @see Cepstrum
 * @see CepstrumFrame
 */
public class CepstralMeanNormalizer extends DataProcessor {


    /**
     * The name of the SphinxProperty for the initial cepstral mean,
     * which has a default value of 12.0F.
     * This is a front-end dependent magic number.
     */
    public static final String PROP_INITIAL_MEAN =
	"edu.cmu.sphinx.frontend.cmn.initialCepstralMean";

    /**
     * The name of the SphinxProperty for the CMN window size,
     * which has a default value of 500.
     */
    public static final String PROP_CMN_WINDOW =
	"edu.cmu.sphinx.frontend.cmn.windowSize";

    /**
     * The name of the SphinxProperty for the CMN shifting window,
     * which has a default value of 800.
     * The shifting window specifies how many cepstrum after which
     * we re-calculate the cepstral mean.
     */
    public static final String PROP_CMN_SHIFT_WINDOW =
	"edu.cmu.sphinx.frontend.cmn.shiftWindow";


    private float initialMean;
    private int cepstrumLength;
    private int numberFrame;
    private float[] currentMean;
    private float[] sum;
    private int cmnShiftWindow;
    private int cmnWindow;

    private Timer normTimer;
    

    /**
     * Constructs a default CepstralMeanNormalizer with the given
     * SphinxProperties context.
     *
     * @param context the context of the SphinxProperties to use
     */
    public CepstralMeanNormalizer(String context) {
        initSphinxProperties(context);
	initMeansSums();
        setTimer(Timer.getTimer(context, "CMN"));
        normTimer = Timer.getTimer(context, "CMN.norm");
    }


    /**
     * Initializes the currentMean and sum arrays.
     */
    private void initMeansSums() {
	currentMean = new float[cepstrumLength];
	currentMean[0] = initialMean;
	sum = new float[cepstrumLength];
    }


    /**
     * Reads the parameters needed from the static SphinxProperties object.
     */
    private void initSphinxProperties(String context) {
	setSphinxProperties(context);
	SphinxProperties properties = getSphinxProperties();
	initialMean = properties.getFloat(PROP_INITIAL_MEAN, 12.0f);
	cepstrumLength = properties.getInt(FrontEnd.PROP_CEPSTRUM_SIZE, 13);
	cmnWindow = properties.getInt(PROP_CMN_WINDOW, 500);
	cmnShiftWindow = properties.getInt(PROP_CMN_SHIFT_WINDOW, 800);
    }
	

    /**
     * Returns the next Data object, which is a normalized CepstrumFrame
     * produced by this class. However, it can also be other Data objects
     * like a EndPointSignal.SEGMENT_START.
     *
     * @return the next available Data object, returns null if no
     *     Data object is available
     *
     * @see CepstrumFrame
     */
    public Data read() throws IOException {
	
        Data input = getSource().read();
        
        if (input instanceof CepstrumFrame) {
            return process((CepstrumFrame) input);
        
        } else if (input instanceof EndPointSignal) {
            
            EndPointSignal signal = (EndPointSignal) input;
            if (signal.equals(EndPointSignal.SEGMENT_END)) {
                updateMeanSumBuffers();
            }
            return input;

        } else {
            return input;
        }
    }	


    /**
     * Normalizes the given CepstrumFrame, which is an array of Cepstrum.
     * Returns the same CepstrumFrame, but with its cepstra normalized.
     * It is assumed that all Cepstrum in the CepstrumFrame are of the same
     * length.
     *
     * @param input a CepstrumFrame
     *
     * @return a normalized CepstrumFrame
     */
    private Data process(CepstrumFrame cepstrumFrame) {
	
        getTimer().start();

        if (cepstrumFrame != null) {

            Cepstrum[] cepstra = cepstrumFrame.getCepstra();
            
            if (cepstra.length > 0) {

                normTimer.start();
                normalize(cepstra);
                normTimer.stop();

                // Shift buffers down if we have more than 
                // cmnShiftWindow frames
                if (numberFrame > cmnShiftWindow) {
                    updateMeanSumBuffers();
                }
            }
	}
	
        getTimer().stop();

	return cepstrumFrame;
    }


    /**
     * Normalize the given array of Cepstrum.
     *
     * @param cepstra the array of Cepstrum to normalize
     */
    private void normalize(Cepstrum[] cepstra) {
	// do the mean normalization
        int cepstraLength = cepstra.length;
	for (int i = 0; i < cepstraLength; i++) {
            normalizeCepstrum(cepstra[i].getCepstrumData());
        }
        numberFrame += cepstra.length;
    }


    /**
     * Normalize the given cepstrum (in the form of float[]) using
     * the sum and currentMean arrays.
     */
    private void normalizeCepstrum(float[] cepstrum) {
        int cepstrumLength = cepstrum.length;
        for (int j = 0; j < cepstrumLength; j++) {
            sum[j] += cepstrum[j];
            cepstrum[j] -= currentMean[j];
        }
    }


    /**
     * Updates the currentMean buffer with the values in the sum buffer.
     * Then decay the sum buffer exponentially, i.e., divide the sum
     * with numberFrames.
     */
    private void updateMeanSumBuffers() {

        if (numberFrame > 0) {
            // update the currentMean buffer with the sum buffer
            float sf = (float) (1.0/numberFrame);
            
            System.arraycopy(sum, 0, currentMean, 0, sum.length);
            
            multiplyArray(currentMean, sf);
            
            // decay the sum buffer exponentially
            if (numberFrame >= cmnShiftWindow) {
                multiplyArray(sum, (sf * cmnWindow));
                numberFrame = cmnWindow;
            }
        }
    }


    /**
     * Multiplies each element of the given array by the multiplier.
     *
     * @param array the array to multiply
     * @param multipler the amount to multiply by
     */
    private static final void multiplyArray(float[] array, float multiplier) {
        int arrayLength = array.length;
        for (int i = 0; i < arrayLength; i++) {
            array[i] *= multiplier;
        }
    }
}
