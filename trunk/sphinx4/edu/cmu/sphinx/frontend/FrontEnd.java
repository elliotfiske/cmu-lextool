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


package edu.cmu.sphinx.frontend;

import edu.cmu.sphinx.util.SphinxProperties;
import edu.cmu.sphinx.util.Timer;

import java.io.IOException;

import java.util.HashMap;
import java.util.Map;

/**
 * Pre-processes the audio into Features. The FrontEnd consists
 * of a series of processors. The FrontEnd connects all the processors
 * by the input and output points.
 * 
 * <p>The input to the FrontEnd is an audio data. For an audio stream,
 * the <code>StreamAudioSource</code> should be used. The 
 * input can also be a file containing a list of audio files, in which case
 * the <code>BatchFileAudioSource</code> can be used.
 *
 * <p>The output of the FrontEnd are Features.
 * To obtain <code>Feature</code>(s) from the FrontEnd, you would use:
 * <pre>
 * fe.getFeatureFrame(10);
 * </pre>
 * which will return a <code>FeatureFrame</code> with 10 <code>Feature</code>s
 * in it. If there are only 5 more features before the end of utterance,
 *
 * This FrontEnd contains all the "standard" frontend processors like
 * Preemphasizer, HammingWindower, SpectrumAnalyzer, ..., etc.
 *
 * @see AudioSource
 * @see BatchCMN
 * @see Feature
 * @see FeatureExtractor
 * @see FeatureFrame
 * @see FeatureSource
 * @see LiveCMN
 * @see MelCepstrumProducer
 * @see MelFilterbank
 * @see SpectrumAnalyzer
 * @see Windower
 */
public class FrontEnd extends DataProcessor {

    private static Map frontends = new HashMap();

    
    /**
     * The prefix for all Frontend SphinxProperties names.
     * Its value is currently <code>"edu.cmu.sphinx.frontend."</code>.
     */
    public static final String PROP_PREFIX = "edu.cmu.sphinx.frontend.";


    /**
     * The SphinxProperty for sample rate in Hertz (i.e.,
     * number of times per second), which has a default value of 8000.
     *
     * NOTE: this property should not have PROP_PREFIX in front of it!
     */
    public static final String PROP_SAMPLE_RATE = "sampleRate";


    /**
     * The SphinxProperty that specifies whether the
     * FrontEnd should use the properties from the AcousticModel.
     */
    public static final String PROP_USE_ACOUSTIC_MODEL_PROPS =
    PROP_PREFIX + "useAcousticModelProperties";

    
    /**
     * The SphinxProperty for the number of bits per sample.
     */
    public static final String PROP_BITS_PER_SAMPLE = 
	PROP_PREFIX + "bitsPerSample";


    /**
     * The SphinxProperty for the number of bytes per frame.
     */
    public static final String PROP_BYTES_PER_AUDIO_FRAME = 
	PROP_PREFIX + "bytesPerAudioFrame";


    /**
     * The SphinxProperty for window size in milliseconds.
     */
    public static final String PROP_WINDOW_SIZE_MS = 
	PROP_PREFIX + "windowSizeInMs";


    /**
     * The SphinxProperty for window shift in milliseconds,
     * which has a default value of 10F.
     */
    public static final String PROP_WINDOW_SHIFT_MS =
	PROP_PREFIX + "windowShiftInMs";

    
    /**
     * The SphinxProperty for the size of a cepstrum, which is
     * 13 by default.
     */
    public static final String PROP_CEPSTRUM_SIZE =
	PROP_PREFIX + "cepstrumSize";


    /**
     * The SphinxProperty that indicates whether Features
     * should retain a reference to the original raw audio bytes. The
     * default value is true.
     */
    public static final String PROP_KEEP_AUDIO_REFERENCE =
	PROP_PREFIX + "keepAudioReference";

    
    /**
     * The SphinxProperty that specifies the Filterbank class.
     */
    public static final String PROP_FILTERBANK = PROP_PREFIX + "filterbank";


    /**
     * The SphinxProperty that specifies the CepstrumProducer class.
     */
    public static final String PROP_CEPSTRUM_PRODUCER = PROP_PREFIX +
	"cepstrumProducer";


    /**
     * The SphinxProperty that specifies the Endpointer class.
     */
    public static final String PROP_ENDPOINTER = PROP_PREFIX + "endpointer";


    /**
     * The SphinxProperty that specifies the CMN class.
     */
    public static final String PROP_CMN = PROP_PREFIX + "cmn";


    private Map processors;
    private AudioSource audioSource;
    private FeatureSource featureSource;
    

    /**
     * Constructs a default FrontEnd.
     *
     * @param name the name of this FrontEnd
     * @param context the context of this FrontEnd
     */
    public FrontEnd(String name, String context, AudioSource audioSource)
        throws IOException {

        super(name, context);
        processors = new HashMap();
        frontends.put(context, this);

        // initialize all the frontend processors

        Preemphasizer preemphasizer = new Preemphasizer
            ("Preemphasizer", context, audioSource);

        Windower windower = new Windower
            ("HammingWindow", context, preemphasizer);

        SpectrumAnalyzer spectrumAnalyzer = new SpectrumAnalyzer
            ("FFT", context, windower);

	Filterbank filterbank = getFilterbank(spectrumAnalyzer);

        CepstrumSource cepstrumProducer = getCepstrumProducer(filterbank);

	CepstrumSource preCMNSource = getEndpointer(cepstrumProducer);

	if (preCMNSource == null) {
	    preCMNSource = cepstrumProducer;
	} else {
	    addProcessor((DataProcessor) preCMNSource);
	}

        CepstrumSource cmn = getCMN(preCMNSource);

        FeatureExtractor extractor = new FeatureExtractor
            ("FeatureExtractor", context, cmn);

        setAudioSource(audioSource);
        setFeatureSource(extractor);

	addProcessor(preemphasizer);
	addProcessor(windower);
	addProcessor(spectrumAnalyzer);
	addProcessor((DataProcessor) filterbank);
	addProcessor((DataProcessor) cepstrumProducer);
	addProcessor((DataProcessor) cmn);
	addProcessor(extractor);
    }


    /**
     * Returns the appropriate Filterbank and initializes it with
     * the given predecessor.
     *
     * @param predecessor the predecessor of this Filterbank
     *
     * @return the appropriate Filterbank
     */
    private Filterbank getFilterbank(SpectrumSource predecessor) {
	String path = null;
	try {
	    path = getSphinxProperties().getString
		(PROP_FILTERBANK, "edu.cmu.sphinx.frontend.mfc.MelFilterbank");
	    Filterbank bank = (Filterbank) Class.forName(path).newInstance();
	    bank.initialize("Filterbank", getContext(), predecessor);
	    return bank;
	} catch (Exception e) {
	    e.printStackTrace();
	    throw new Error("Can't create Filterbank: " + e);
	}
    }


    /**
     * Returns the appropriate CepstrumProducer, and initializes it
     * with the given predecessor.
     *
     * @param predecessor the predecessor
     *
     * @return the appropriate CepstrumProducer
     */
    private CepstrumProducer getCepstrumProducer(SpectrumSource predecessor) {
	String path = null;
	try {
	    path = getSphinxProperties().getString
		(PROP_CEPSTRUM_PRODUCER,
		 "edu.cmu.sphinx.frontend.mfc.MelCepstrumProducer");
	    CepstrumProducer producer = 
		(CepstrumProducer) Class.forName(path).newInstance();
	    producer.initialize("CepstrumProducer", getContext(), predecessor);
	    return producer;
	} catch (Exception e) {
	    e.printStackTrace();
	    throw new Error("Can't create CepstrumProducer: " + e);
	}
    }


    /**
     * Returns the appropriate Endpointer, if any, and initializes it
     * with the given predecessor.
     *
     * @param predecessor the predecessor of this Endpointer
     */
    private CepstrumSource getEndpointer(CepstrumSource predecessor) {
        String path = getSphinxProperties().getString(PROP_ENDPOINTER, null);

        if (path != null) {
	    CepstrumSource endpointer = null;

	    /*
            EnergyEndpointer energyEndpointer = new EnergyEndpointer
                ("EnergyEndpointer", context, melCepstrum);
            NonSpeechFilter nonSpeechFilter = new NonSpeechFilter
                ("NonSpeechFilter", context, energyEndpointer);
	    */

	    return endpointer;
        } else {
	    return null;
	}
    }


    /**
     * Returns the appropriate Cepstral Mean Normalizer (CMN)
     * as specified in the SphinxProperties, with the given predecessor.
     *
     * @param predecessor the predecessor of the CMN step
     */
    private CepstrumSource getCMN(CepstrumSource predecessor) throws 
	IOException {
	String path = getSphinxProperties().getString
	    (PROP_CMN, "edu.cmu.sphinx.frontend.BatchCMN");

	CepstrumSource cmn = null;

	if (path.equals("edu.cmu.sphinx.frontend.LiveCMN")) {
	    cmn = new LiveCMN("LiveCMN", getContext(), predecessor);
	} else if (path.equals("edu.cmu.sphinx.frontend.BatchCMN")) {
	    cmn = new BatchCMN("BatchCMN", getContext(), predecessor);
	}
	return cmn;
    }


    /**
     * Adds the given DataProcessor into the list of processors,
     * with the name of the processor as key.
     *
     * @param processor the DataProcessor to add
     *
     * @return the added DataProcessor
     */
    private DataProcessor addProcessor(DataProcessor processor) {
        processors.put(processor.getName(), processor);
        return processor;
    }


    /**
     * Returns the DataProcessor in the FrontEnd with the given name.
     *
     * @return the DataProcessor with the given name, or null if no
     *    such DataProcessor
     */
    public DataProcessor getProcessor(String processorName) {
        Object object = processors.get(processorName);
        if (object == null) {
            return null;
        } else {
            return (DataProcessor) object;
        }
    }


    /**
     * Returns the FrontEnd with the given context, or null if there is no
     * FrontEnd with that context.
     *
     * @return the FrontEnd with the given context, or null if there
     *    is no FrontEnd with that context
     */
    public static FrontEnd getFrontEnd(String context) {
        Object frontend = frontends.get(context);
        if (frontend != null) {
            return (FrontEnd) frontend;
        } else {
            return null;
        }
    }


    /**
     * Returns the AudioSource of this FrontEnd. The AudioSource of
     * the front end is where it gets its audio data.
     *
     * @return the AudioSource
     */
    private AudioSource getAudioSource() {
        return audioSource;
    }


    /**
     * Sets the AudioSource of this FrontEnd.  The AudioSource of
     * the front end is where it gets its audio data
     *
     * @param audioSource the AudioSource
     */
    public void setAudioSource(AudioSource audioSource) {
        this.audioSource = audioSource;
    }


    /**
     * Returns the FeatureSource of this FrontEnd.
     *
     * @return the FeatureSource of this FrontEnd
     */
    private FeatureSource getFeatureSource() {
        return featureSource;
    }


    /**
     * Sets the FeatureSource of this FrontEnd.
     *
     * @param featureSource the FeatureSource
     */
    private void setFeatureSource(FeatureSource featureSource) {
        this.featureSource = featureSource;
    }


    /**
     * Returns the next N number of Features produced by this FrontEnd
     * as a single FeatureFrame.
     * The number of Features return maybe less than N, in which
     * case the last Feature will contain a Signal.UTTERANCE_END signal.
     * Consequently, the size of the FeatureFrame will be less than N.
     *
     * @param numberFeatures the number of Features to return
     *
     * @return the next N number of Features in a FeatureFrame, or null
     *    if no more FeatureFrames available
     *
     * @see FeatureFrame
     */
    public FeatureFrame getFeatureFrame(int numberFeatures) throws
    IOException {

        getTimer().start();

        FeatureFrame featureFrame = null;
        Feature[] features = new Feature[numberFeatures];
        Feature feature = null;

        int i = 0;
        do {
            feature = featureSource.getFeature();
            if (feature != null) {
                features[i++] = feature;
                if (feature.hasSignal(Signal.UTTERANCE_END)) {
                    break;
                }
            } else {
                break;
            }
        } while (i < numberFeatures);

        // if there are some features
        if (i > 0) {

            // if we hit the end of utterance before getting the
            // desired number of features, shrink the array
            if (i < numberFeatures) {
                Feature[] lessFeatures = new Feature[i];
                for (i = 0; i < lessFeatures.length; i++) {
                    lessFeatures[i] = features[i];
                }
                features = lessFeatures;
            }
            
            featureFrame = new FeatureFrame(features);
        }
        
        getTimer().stop();

        return featureFrame;
    }

}
