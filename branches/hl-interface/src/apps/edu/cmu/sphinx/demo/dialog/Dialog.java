/*
 * Copyright 2013 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 */

package edu.cmu.sphinx.demo.dialog;

import java.net.URL;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import edu.cmu.sphinx.api.Configuration;
import edu.cmu.sphinx.api.RecognitionResult;
import edu.cmu.sphinx.api.LiveSpeechRecognizer;


abstract class Menu {

    private final String name;
    private final List<Menu> children = new ArrayList<Menu>();
    private final Map<String, Menu> tags = new HashMap<String, Menu>();
    protected final List<String> captions = new ArrayList<String>();

    protected final LiveSpeechRecognizer recognizer;

    public Menu(String name, Configuration configuration) {
        this.name = name;
        recognizer = new LiveSpeechRecognizer(configuration);
    }

    public void append(String tag, Menu menu) {
        children.add(menu);
        tags.put(tag, menu);
    }

    public void enter() {
        while (true) {
            show();
            recognizer.startRecognition(true);
            RecognitionResult result = recognizer.getResult();

            if (children.isEmpty()) {
                while (onCommand(result))
                    result = recognizer.getResult();

                recognizer.stopRecognition();
                break;
            } else {
                String tag = result.getUtterance(false);
                recognizer.stopRecognition();
                if (tags.containsKey(tag))
                    tags.get(tag).enter();
                else if (tag.startsWith("exit"))
                    break;
                else
                    System.out.println("No such category: " +
                            result.getUtterance(false));
            }
        }
    }

    public void show() {
        int maxlen = name.length() + 2;
        for (String s : captions)
            maxlen = Math.max(maxlen, s.length() + 2);

        String hrule = "";
        String hspace = "";
        for (int i = 0; i < maxlen; ++i) {
            hrule += "-";
            hspace += " ";
        }

        StringBuffer sb = new StringBuffer(hspace);
        sb.replace((maxlen - name.length()) / 2,
                (maxlen + name.length()) / 2, name);
        sb.insert(0, '|');
        sb.append('|');
        System.out.println("+" + hrule + "+");
        System.out.println(sb.toString());
        System.out.println("+" + hrule + "+");

        for (String s : captions) {
            sb = new StringBuffer(hspace);
            sb.replace((maxlen - s.length()) / 2,
                    (maxlen + s.length()) / 2, s);
            sb.insert(0, '|');
            sb.append('|');
            System.out.println(sb.toString());
        }

        if (!captions.isEmpty())
            System.out.println("+" + hrule + "+");
    }

    protected abstract boolean onCommand(RecognitionResult result);
}

class MainMenu extends Menu {

    public MainMenu(Configuration configuration) {
        super("Voice menu", configuration);
        captions.add("Digits");
        captions.add("Bank account");
        captions.add("Weather forecast");
        captions.add("Exit");
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        return false;
    }
}

class DigitsMenu extends Menu {

    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/dialog/";

    public DigitsMenu(Configuration configuration) {
        super("Digits (using GrXML)", configuration);
        captions.add("Example: one two three");
        captions.add("Say \"101\" to exit");
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        String utt = result.getUtterance(false);
        if (utt.equals("one oh one") || utt.equals("one zero one"))
            return false;

        System.out.println(utt);
        return true;
    }
}

class BankMenu extends Menu {

    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/dialog/";

    private static final Map<String, Integer> DIGITS =
        new HashMap<String, Integer>();

    static {
        DIGITS.put("oh", 0);
        DIGITS.put("zero", 0);
        DIGITS.put("one", 1);
        DIGITS.put("two", 2);
        DIGITS.put("three", 3);
        DIGITS.put("four", 4);
        DIGITS.put("five", 5);
        DIGITS.put("six", 6);
        DIGITS.put("seven", 7);
        DIGITS.put("eight", 8);
        DIGITS.put("nine", 9);
    }

    private double savings;

    public BankMenu(Configuration configuration) {
        super("Bank account", configuration);
        captions.add("Example: balance                 ");
        captions.add("Example: withdraw zero point five");
        captions.add("Example: deposit one two three   ");
        captions.add("Example: back                    ");
    }

    @Override
    public void enter() {
        savings = .0;
        super.enter();
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        String hypothesis = result.getUtterance(false);
        if (hypothesis.equals("back")) {
            return false;
        } else if (hypothesis.startsWith("deposit")) {
            double deposit = parseNumber(hypothesis.split("\\s"));
            savings += deposit;
            System.out.format("Deposited: $%.2f\n", deposit);
        } else if (hypothesis.startsWith("withdraw")) {
            double withdraw = parseNumber(hypothesis.split("\\s"));
            savings -= withdraw;
            System.out.format("Withdrawn: $%.2f\n", withdraw);
        }

        System.out.format("Your savings: $%.2f\n", savings);
        return true;
    }

    private static double parseNumber(String[] tokens) {
        StringBuilder sb = new StringBuilder();

        for (int i = 1; i < tokens.length; ++i) {
            if (tokens[i].equals("point"))
                sb.append(".");
            else
                sb.append(DIGITS.get(tokens[i]));
        }

        return Double.parseDouble(sb.toString());
    }
}

class WeatherMenu extends Menu {


    public WeatherMenu(Configuration configuration) {
        super("Try some forecast. End with \"the end\"", configuration);
        captions.add("Example: mostly dry some fog patches tonight");
        captions.add("Example: sunny spells on wednesday          ");
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        String hypothesis = result.getUtterance(false);
        while (!hypothesis.equals("the end")) {
            System.out.println(hypothesis);
            return true;
        }

        return false;
    }
}

public class Dialog {

    private static final String ACOUSTIC_MODEL =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz";
    private static final String DICTIONARY_PATH =
        "resource:/WSJ_8gau_13dCep_16k_40mel_130Hz_6800Hz/dict/cmudict.0.6d";
    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/dialog/";
    private static final String LANGUAGE_MODEL =
        "resource:/edu/cmu/sphinx/demo/dialog/weather.lm";


    public static void main(String[] args) throws Exception {
        Configuration configuration = new Configuration();
        configuration.setAcousticModelPath(ACOUSTIC_MODEL);
        configuration.setDictionaryPath(DICTIONARY_PATH);
        configuration.setGrammarPath(GRAMMAR_PATH);
        configuration.setUseGrammar(true);

        configuration.setGrammarName("menu");
        Menu menu = new MainMenu(configuration);

        configuration.setGrammarName("digits.grxml");
        menu.append("digits", new DigitsMenu(configuration));

        configuration.setGrammarName("bank");
        menu.append("bank account", new BankMenu(configuration));

        configuration.setUseGrammar(false);
        configuration.setLanguageModelPath(LANGUAGE_MODEL);
        menu.append("weather forecast", new WeatherMenu(configuration));

        menu.enter();
    }
}
