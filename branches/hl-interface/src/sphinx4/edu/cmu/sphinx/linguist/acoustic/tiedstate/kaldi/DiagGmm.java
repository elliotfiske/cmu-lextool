package edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi;

import java.util.Arrays;
import java.util.List;

import edu.cmu.sphinx.frontend.Data;
import edu.cmu.sphinx.frontend.FloatData;

import edu.cmu.sphinx.linguist.acoustic.tiedstate.ScoreCachingSenone;

import edu.cmu.sphinx.util.LogMath;

/**
 *
 * @see DiagGmm class in Kaldi.
 */
public class DiagGmm extends ScoreCachingSenone {

    private long id;
    private float[] gconsts;
    private float[] invVars;
    private float[] meansInvVars;

    public DiagGmm(long id,
                   List<Float> gconsts,
                   List<Float> meansInvVars,
                   List<Float> invVars)
    {
        this.id = id;
        this.gconsts = asFloatArray(gconsts);
        this.meansInvVars = asFloatArray(meansInvVars);
        this.invVars = asFloatArray(invVars);
    }

    @Override
    public float calculateScore(Data data) {
        float[] scores = calculateComponentScore(data);
        float logTotal = LogMath.LOG_ZERO;
        LogMath logMath = LogMath.getInstance();
        for (Float mixtureScore : calculateComponentScore(data))
            logTotal = logMath.addAsLinear(logTotal, mixtureScore);

        return logTotal;
    }

    @Override
    public float[] calculateComponentScore(Data data) {
        float[] features = FloatData.toFloatData(data).getValues();
        int dim = meansInvVars.length / gconsts.length;
        if (features.length != dim) {
            String fmt = "feature vector must be of length %d, got %d";
            String msg = String.format(fmt, dim, features.length);
            throw new IllegalArgumentException(msg);
        }

        float[] likelihoods = Arrays.copyOf(gconsts, gconsts.length);
        for (int i = 0; i < likelihoods.length; ++i) {
            for (int j = 0; j < features.length; ++j) {
                int k = i * features.length + j;
                likelihoods[i] += meansInvVars[k] * features[j];
                likelihoods[i] -= .5f * invVars[k] * features[j] * features[j];
            }

            likelihoods[i] = LogMath.getInstance().lnToLog(likelihoods[i]);
        }

        return likelihoods;
    }

    @Override
    public long getID() {
        return id;
    }

    @Override
    public void dump(String msg) {
        System.out.format("%s DiagGmm: ID %d\n", msg, id);
    }

    private static float[] asFloatArray(List<Float> list) {
        float[] array = new float[list.size()];
        int i = 0;
        for (Float n : list)
            array[i++] = n;

        return array;
    }
}
