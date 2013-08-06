package edu.cmu.sphinx.api;

import java.net.MalformedURLException;
import java.net.URL;

import java.util.List;

import edu.cmu.sphinx.linguist.acoustic.AcousticModel;
import edu.cmu.sphinx.linguist.language.ngram.LanguageModel;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.result.WordResult;
import edu.cmu.sphinx.util.props.ConfigurationManager;

import static edu.cmu.sphinx.util.props.ConfigurationManagerUtils.resourceToURL;


/**
 * Base class for Sphinx4 high-level interface.
 */
public abstract class AbstractSpeechRecognizer {

    private final ConfigurationManager configurationManager;
    private final Recognizer recognizer;
    private Result result;

    protected AbstractSpeechRecognizer(ConfigurationManager cm) {
        recognizer = (Recognizer) cm.lookup("recognizer");
        configurationManager = cm;

        recognizer.allocate();
    }

    private static ConfigurationManager getConfigurationManager() {
        String path = "resource:/edu/cmu/sphinx/config/main.config.xml";
        URL url = null;
        try {
            url = resourceToURL(path);
        } catch (MalformedURLException e) {
            throw new IllegalStateException(path + " not found", e);
        }

        return new ConfigurationManager(url);
    }

    public static AbstractSpeechRecognizer createLiveRecognizer() {
        ConfigurationManager cm = getConfigurationManager();
        AbstractSpeechRecognizer recognizer = new LiveSpeechRecognizer(cm);
        return recognizer;
    }

    public static AbstractSpeechRecognizer createResourceRecognizer(
            URL resource)
    {
        ConfigurationManager cm = getConfigurationManager();
        AbstractSpeechRecognizer recognizer =
            new ResourceSpeechRecognizer(cm, resource);
        return recognizer;
    }

    public String getBestResult() {
        return result.getBestResultNoFiller();
    }

    public String getBestFinalResult() {
        return result.getBestFinalResultNoFiller();
    }

    public List<WordResult> getWords() {
        return result.getWords();
    }

    public void setAcousticModel(AcousticModel model) {
        // FIXME: implement
    }

    /**
     * Set search path for grammar files.
     *
     * This will enable fixed grammar and disable language model.
     */
    public void setGrammarLocation(URL location) {
        // FIXME: set grammar seach path.
        configurationManager.setGlobalProperty("linguist", "flatLinguist");
    }

    /**
     * Set language model.
     *
     * This will disable fixed grammar.
     */
    public void setLanguageModel(LanguageModel model) {
        // FIXME: set model
        configurationManager.setGlobalProperty("linguist", "lexTreeLinguist");
    }

    public abstract void startRecognition();

    public abstract void stopRecognition();
}
