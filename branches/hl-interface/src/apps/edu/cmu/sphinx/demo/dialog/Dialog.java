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

import java.io.IOException;

import java.net.MalformedURLException;
import java.net.URL;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import edu.cmu.sphinx.api.SimpleSpeechRecognizer;
import edu.cmu.sphinx.api.RecognitionResult;
import edu.cmu.sphinx.jsgf.JSGFGrammarException;
import edu.cmu.sphinx.jsgf.JSGFGrammarParseException;


abstract class DialogMenu {

    private final String name;
    private final List<DialogMenu> children = new ArrayList<DialogMenu>();
    private final Map<String, DialogMenu> tags = new HashMap<String, DialogMenu>();

    protected final List<String> captions = new ArrayList<String>();

    public DialogMenu(String name) {
        this.name = name;
    }

    public void append(String tag, DialogMenu menu) {
        children.add(menu);
        tags.put(tag, menu);
    }

    public void enter(SimpleSpeechRecognizer recognizer) {
        while (true) {
            show();
            onEnter(recognizer);
            recognizer.startRecognition(true);
            RecognitionResult result = recognizer.getResult();

            if (children.isEmpty()) {
                while (onCommand(result))
                    result = recognizer.getResult();

                recognizer.stopRecognition();
                break;
            } else {
                String tag = result.getBestFinalResult();
                recognizer.stopRecognition();
                if (tags.containsKey(tag))
                    tags.get(tag).enter(recognizer);
                else if (tag.startsWith("exit"))
                    break;
                else
                    System.out.println("No such category: " +
                            result.getBestFinalResult());
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

    protected abstract void onEnter(SimpleSpeechRecognizer recognizer);

    protected abstract boolean onCommand(RecognitionResult result);
}

class MainMenu extends DialogMenu {

    private static final String GRAMMAR_PATH =
        "resource:/edu/cmu/sphinx/demo/dialog/";

    public MainMenu() {
        super("Main menu");
        captions.add("Bank account");
        captions.add("Weather forecast");
        captions.add("Help");
        captions.add("Exit");
    }

    @Override
    protected void onEnter(SimpleSpeechRecognizer recognizer) {
        recognizer.setGrammar(GRAMMAR_PATH, "menu");
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        return false;
    }
}

class BankMenu extends DialogMenu {

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

    public BankMenu() {
        super("Bank account");
        captions.add("Balance");
        captions.add("Deposit");
        captions.add("Withdraw");
        captions.add("Help");
        captions.add("Back");
    }

    @Override
    protected void onEnter(SimpleSpeechRecognizer recognizer) {
        recognizer.setGrammar(GRAMMAR_PATH, "bank");
        savings = .0;
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        String hypothesis = result.getBestFinalResult();
        if (hypothesis.equals("back")) {
            return false;
        } else if (hypothesis.endsWith("help")) {
            // FIXME: implement
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

class WeatherMenu extends DialogMenu {

    private static final String LANGUAGE_MODEL =
        "resource:/edu/cmu/sphinx/demo/dialog/weather.lm";

    public WeatherMenu() {
        super("Try some forecast. End with \"the end\"");
    }

    @Override
    protected void onEnter(SimpleSpeechRecognizer recognizer) {
        recognizer.setLanguageModel(LANGUAGE_MODEL);
    }

    @Override
    protected boolean onCommand(RecognitionResult result) {
        String hypothesis = result.getBestResult();
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

    public static void main(String[] args)
        throws IOException, MalformedURLException, JSGFGrammarException, JSGFGrammarParseException
    {
        SimpleSpeechRecognizer recognizer = new SimpleSpeechRecognizer();
        recognizer.setAcousticModel(ACOUSTIC_MODEL);
        recognizer.setDictionary(DICTIONARY_PATH);

        DialogMenu menu = new MainMenu();
        menu.append("bank account", new BankMenu());
        menu.append("weather forecast", new WeatherMenu());
        menu.enter(recognizer);
    }
}
