/**
 * [[[copyright]]]
 */

package edu.cmu.sphinx.frontend;

/**
 * Represents the power spectrum.
 */
public class Spectrum implements Data {

    private double[] spectraData;


    /**
     * Constructs a Spectrum with the given spectra data.
     *
     * @param spectraData a frame's spectral data
     */
    public Spectrum(double[] spectraData) {
	this.spectraData = spectraData;
    }


    /**
     * Returns the spectrum data.
     *
     * @return the spectrum data
     */
    public double[] getSpectrumData() {
	return spectraData;
    }

    
    /**
     * Returns a string representation of this Spectrum.
     *
     * @return a string representation of this Spectrum
     */ 
    public String toString() {
        return ("SPEC_MAGNITUDE " + Util.doubleArrayToString(spectraData));
    }                
}
