package edu.cmu.sphinx.api;

import edu.cmu.sphinx.frontend.util.Microphone;
import edu.cmu.sphinx.util.props.ConfigurationManager;

/**
 * High-level interface for Sphinx4 live recognition.
 */
public class LiveSpeechRecognizer extends AbstractSpeechRecognizer {

    private final Microphone microphone;

    /**
     * Instance should be obstained using static methods of
     * AbstractSpeechRecognizer.
     */
    LiveSpeechRecognizer(ConfigurationManager cm) {
        super(cm);
        microphone = (Microphone) cm.lookup("microphone");
    }

    @Override
    public void startRecognition() {
        microphone.startRecording();
    }

    @Override
    public void stopRecognition() {
        microphone.stopRecording();
    }
}
