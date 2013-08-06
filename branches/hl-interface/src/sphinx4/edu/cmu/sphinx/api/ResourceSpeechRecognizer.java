package edu.cmu.sphinx.api;

import java.net.URL;

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.util.props.ConfigurationManager;


/**
 * High-level interface for Sphinx4 live recognition.
 */
public class ResourceSpeechRecognizer extends AbstractSpeechRecognizer {

    AudioFileDataSource dataSource;

    /**
     * Instance should be obstained using static methods of
     * AbstractSpeechRecognizer.
     */
    ResourceSpeechRecognizer(ConfigurationManager cm, URL resource) {
        super(cm);
        dataSource = (AudioFileDataSource) cm.lookup("audioFileDataSource");
        dataSource.setAudioFile(resource, null);
    }

    /**
     * Does nothing.
     */
    @Override
    public void startRecognition() {
    }

    /**
     * Does nothing.
     */
    @Override
    public void stopRecognition() {
    }
}
