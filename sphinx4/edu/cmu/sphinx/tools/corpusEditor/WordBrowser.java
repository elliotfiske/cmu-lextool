package edu.cmu.sphinx.tools.corpusEditor;

import edu.cmu.sphinx.tools.audio.AudioData;
import edu.cmu.sphinx.util.props.ConfigurationManager;
import edu.cmu.sphinx.util.props.PropertyException;

import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.List;

/**
 * Copyright 1999-2006 Carnegie Mellon University.
 * Portions Copyright 2002 Sun Microsystems, Inc.
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * <p/>
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 * <p/>
 * User: Peter Wolf
 * Date: Jan 25, 2006
 * Time: 3:02:10 PM
 */
public class WordBrowser {
    WordSpectrogramPanel spectrogram;
    private JCheckBox excluded;
    private JButton play;
    JPanel mainPane;
    JSlider zoom;

            WordBrowser() {
                    zoom.addChangeListener(new ChangeListener() {
                public void stateChanged(ChangeEvent event) {
                    JSlider slider = (JSlider) event.getSource();
                    int max = slider.getMaximum();
                    int min = slider.getMinimum();
                    int val = slider.getValue();
                    //double z = Math.pow(100.0,((double)val)/(max-min));
                    //w.spectrogram.setZoom(z);
                    spectrogram.setOffset( ((double)val)/(max-min) );
                }
            });
    }


    public static void main(String[] args) {

        if (args.length != 3) {
            System.out.println(
                    "Usage: WordBrowser propertiesFile corpusFile word");
            System.exit(1);
        }

        String propertiesFile = args[0];
        String corpusFile = args[1];
        String spelling = args[2];

        try {
            URL url = new File(propertiesFile).toURI().toURL();
            ConfigurationManager cm = new ConfigurationManager(url);

            Corpus corpus = CorpusBuilder.readCorpus(corpusFile);

            List<Word> words = corpus.getWords(spelling);

            Word word = words.get(0);

            final WordBrowser w = new WordBrowser();
            JFrame f = new JFrame("WordBrowser");
            w.spectrogram.setWord(cm,word);

            f.setContentPane(w.mainPane);
            f.pack();
            f.setDefaultCloseOperation(JFrame.DISPOSE_ON_CLOSE);

            f.setVisible(true);



        } catch (MalformedURLException e) {
            throw new Error(e);
        } catch (PropertyException e) {
            throw new Error(e);
        } catch (IOException e) {
            throw new Error(e);
        }
    }

    void play( AudioData data ) {
        /*
        AudioFileWriter out = new AudioFileWriter()
        AudioClip ac = Applet.getAudioClip();
        */
    }


    {
// GUI initializer generated by IntelliJ IDEA GUI Designer
// >>> IMPORTANT!! <<<
// DO NOT EDIT OR ADD ANY CODE HERE!
        $$$setupUI$$$();
    }

    /**
     * Method generated by IntelliJ IDEA GUI Designer
     * >>> IMPORTANT!! <<<
     * DO NOT edit this method OR call it in your code!
     */
    private void $$$setupUI$$$() {
        mainPane = new JPanel();
        mainPane.setLayout(new com.intellij.uiDesigner.core.GridLayoutManager(2, 3, new Insets(0, 0, 0, 0), -1, -1));
        excluded = new JCheckBox();
        excluded.setText("Excluded");
        mainPane.add(excluded, new com.intellij.uiDesigner.core.GridConstraints(1, 0, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null));
        play = new JButton();
        play.setText("Play");
        mainPane.add(play, new com.intellij.uiDesigner.core.GridConstraints(1, 2, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_SHRINK | com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_CAN_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null));
        zoom = new JSlider();
        zoom.setValue(0);
        mainPane.add(zoom, new com.intellij.uiDesigner.core.GridConstraints(1, 1, 1, 1, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_WEST, com.intellij.uiDesigner.core.GridConstraints.FILL_HORIZONTAL, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_WANT_GROW, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, null, null, null));
        spectrogram = new WordSpectrogramPanel();
        mainPane.add(spectrogram, new com.intellij.uiDesigner.core.GridConstraints(0, 0, 1, 3, com.intellij.uiDesigner.core.GridConstraints.ANCHOR_CENTER, com.intellij.uiDesigner.core.GridConstraints.FILL_NONE, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, com.intellij.uiDesigner.core.GridConstraints.SIZEPOLICY_FIXED, new Dimension(512, 256), null, null));
    }
}
