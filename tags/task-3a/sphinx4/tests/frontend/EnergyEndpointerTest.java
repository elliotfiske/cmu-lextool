/**
 * [[[copyright]]]
 */

package tests.frontend;

import edu.cmu.sphinx.frontend.Cepstrum;
import edu.cmu.sphinx.frontend.CepstrumSource;
import edu.cmu.sphinx.frontend.EnergyEndpointer;
import edu.cmu.sphinx.frontend.FrontEnd;
import edu.cmu.sphinx.frontend.MelCepstrumProducer;
import edu.cmu.sphinx.frontend.MelFilterbank;
import edu.cmu.sphinx.frontend.NonSpeechFilter;
import edu.cmu.sphinx.frontend.Preemphasizer;
import edu.cmu.sphinx.frontend.Signal;
import edu.cmu.sphinx.frontend.SpectrumAnalyzer;
import edu.cmu.sphinx.frontend.Utterance;
import edu.cmu.sphinx.frontend.Windower;

import java.util.*;

/**
 * Test program for the MelFilterbank.
 */
public class EnergyEndpointerTest {

    public static void main(String[] argv) {

	if (argv.length < 3) {
	    System.out.println
                ("Usage: java testClass <testName> " +
                 "<propertiesFile> <audiofilename>");
	}

        try {
            String testName = argv[0];
            String propertiesFile = argv[1];
            String audioFile = argv[2];

            ProcessorTest fet = new ProcessorTest
                (testName, propertiesFile, audioFile);

            Preemphasizer preemphasizer = new Preemphasizer
                ("Preemphasizer", testName, fet.getAudioSource());
            Windower windower = new Windower
                ("HammingWindow", testName, preemphasizer);
            SpectrumAnalyzer spectrumAnalyzer = new SpectrumAnalyzer
                ("FFT", testName, windower);
            MelFilterbank melFilterbank = new MelFilterbank
                ("MelFilter", testName, spectrumAnalyzer);
            MelCepstrumProducer melCepstrum = new MelCepstrumProducer
                ("MelCepstrumProducer", testName, melFilterbank);
	    EnergyEndpointer endpointer = new EnergyEndpointer
                ("EnergyEndpointer", testName, melCepstrum);
            NonSpeechFilter nonSpeechFilter = new NonSpeechFilter
                ("NonSpeechFilter", testName, endpointer);

            CepstrumSource finalSource = endpointer;
            if (Boolean.getBoolean("removeNonSpeech")) {
                finalSource = nonSpeechFilter;
            }

            final CepstraViewer cepstraViewer = 
                new CepstraViewer("EndpointerTest");
            cepstraViewer.show();

            CepstraGroupProducer groupProducer = new CepstraGroupProducer
                ("CepstraGroupProducer", testName, finalSource) {
                
                public void cepstraGroupProduced(CepstraGroup cepstraGroup) {
                    cepstraViewer.addCepstraGroup(cepstraGroup);
                }
            };

            Cepstrum cepstrum = null;
            do {
                cepstrum = groupProducer.getCepstrum();
            } while (cepstrum != null);

            System.out.println("no more cepstrum");

	} catch (Exception e) {
	    e.printStackTrace();
	}
    }
}