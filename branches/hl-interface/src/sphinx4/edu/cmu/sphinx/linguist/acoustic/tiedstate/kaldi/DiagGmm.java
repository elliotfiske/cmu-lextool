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
                   List<Float> invVars,
                   List<Float> meansInvVars)
    {
        this.id = id;
        this.gconsts = asFloatArray(gconsts);
        this.invVars = asFloatArray(invVars);
        this.meansInvVars = asFloatArray(meansInvVars);
    }

    @Override
    public float calculateScore(Data data) {
        LogMath logMath = LogMath.getInstance();
        // float logTotal = LogMath.LOG_ZERO;
        float[] scores = calculateComponentScore(data);
        float logTotal = scores[0];
        //for (Float mixtureScore : calculateComponentScore(data)) {
        for (int i = 1; i < scores.length; ++i) {
            //logTotal = logTotal +
            //           (float) Math.log(1 + Math.exp(scores[i] - logTotal));
            logTotal = logMath.addAsLinear(logTotal, scores[i]);
            // System.out.format("score: %f, total:%f\n", mixtureScore, logTotal);
            // logTotal = logMath.addAsLinear(logTotal, mixtureScore);
            //logTotal = (float) Math.log(Math.exp(logTotal) + Math.exp(mixtureScore));
        }

        System.out.format("%d log-total: %f\n", getID(), logTotal);
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
                likelihoods[i] += features[j] * meansInvVars[k];
                likelihoods[i] -= .5f * invVars[k] * features[j] * features[j];
                // likelihoods[i] += features[j] * (meansInvVars[k] - .5 * invVars[k] * features[j]);
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
