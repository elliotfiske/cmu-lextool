
/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */


package edu.cmu.sphinx.frontend.util;

import edu.cmu.sphinx.frontend.Cepstrum;
import edu.cmu.sphinx.frontend.Signal;

import edu.cmu.sphinx.util.SphinxProperties;

import java.util.Arrays;


/**
 * Plots positive energy values of Cepstrum to stdout. 
 * An exception will be thrown if the given energy is negative.
 * The plots look like the following, one line per Cepstrum. The
 * energy value for that particular Cepstrum is printed at the end of
 * the line.
 * <p>
 * <code>
<br>......7
<br>......7
<br>Cepstrum: SPEECH_START
<br>......7
<br>.......8
<br>......7
<br>.......8
<br>.......8
<br>........9
<br>............14
<br>...........13
<br>...........13
<br>...........13
<br>.............15
<br>.............15
<br>..............16
<br>..............16
<br>..............16
<br>.............15
<br>............14
<br>............14
<br>............14
<br>............14
<br>.............15
<br>..............16
<br>...............17
<br>...............17
<br>...............17
<br>...............17
<br>...............17
<br>...............17
<br>..............16
<br>.............15
<br>............14
<br>............14
<br>............14
<br>...........13
<br>........9
<br>.......8
<br>......7
<br>......7
<br>......7
<br>Cepstrum: SPEECH_END
<br>......7
</code>
 */
public class EnergyPlotter {

    public static final String PROP_MAX_ENERGY =
    "edu.cmu.sphinx.frontend.util.EnergyPlotter.maxEnergy";

    public static final int PROP_MAX_ENERGY_DEFAULT = 20;


    private int maxEnergy;
    private String[] plots;


    /**
     * Constructs an EnergyPlotter.
     *
     * @param maxEnergy the maximum energy value
     */
    public EnergyPlotter(SphinxProperties props) {
        maxEnergy = props.getInt(PROP_MAX_ENERGY, PROP_MAX_ENERGY_DEFAULT);
        buildPlots(maxEnergy);
    }


    /**
     * Builds the strings for the plots.
     *
     * @param maxEnergy the maximum energy value
     */
    private void buildPlots(int maxEnergy) {
        plots = new String[maxEnergy+1];
        for (int i = 0; i < maxEnergy+1; i++) {
            plots[i] = getPlotString(i);
        }
    }


    /**
     * Returns the plot string for the given energy.
     *
     * @param energy the energy level
     */
    private String getPlotString(int energy) {
        char[] plot = new char[energy];
        Arrays.fill(plot, '.');
        if (energy > 0) {
            if (energy < 10) {
                plot[plot.length - 1] = (char) ('0' + energy);
            } else {
                plot[plot.length - 2] = '1';
                plot[plot.length - 1] = (char) ('0' + (energy - 10));
            }
        }
        return (new String(plot));
    }

    
    /**
     * Plots the energy values of the given Cepstrum to System.out.
     * If the Cepstrum contains a signal, it prints the signal.
     *
     * @param cepstrum the Cepstrum to plot
     */
    public void plot(Cepstrum cepstrum) {
        if (cepstrum != null) {
            if (cepstrum.hasContent()) {
                int energy = (int) cepstrum.getEnergy();
                System.out.println(getPlot(energy));
            } else {
                System.out.println(cepstrum.toString());
            }
        }
    }


    /**
     * Returns the corresponding plot String for the given energy value.
     * The energy value must be positive, otherwise an 
     * IllegalArgumentException will be thrown.
     *
     * @return energy the energy value
     */
    private String getPlot(int energy) {
        if (energy < 0) {
            throw new IllegalArgumentException
                ("Negative energy encountered: " + energy);
        }
        if (energy <= maxEnergy) {
            return plots[energy];
        } else {
            return getPlotString(energy);
        }
    }
}
