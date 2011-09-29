package edu.cmu.sphinx.demo.aligner;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.linguist.aflat.AFlatLinguist;
import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.recognizer.Recognizer.State;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.StringCustomise;
import edu.cmu.sphinx.util.StringErrorGenerator;
import edu.cmu.sphinx.util.props.ConfigurationManager;

public class Aligner implements AudioAlignerInterface{
	
	private String PROP_GRAMMAR;	// which grammar to use from config
	private String PROP_RECOGNIZER;	// which recognizer to use from config
	private String PROP_GRAMMAR_TYPE;
	private String PROP_AUDIO_DATA_SOURCE;
	
	private String absoluteBeamWidth;
	private String relativeBeamWidth;
	private String outOfGrammarProbability;
	private String phoneInsertionProbability;
	
	private ConfigurationManager cm;
	private Recognizer recognizer;
	private AlignerGrammar grammar;
	private AudioFileDataSource datasource;
	
	private boolean optimize;	// by default set this false
	
	private String config;
	private String audioFile;
	private String textFile;
	private String txtInTranscription;
	
	public Aligner(String config, String audioFile, String textFile)
			throws IOException {
		this(config, audioFile, textFile, "recognizer", "AlignerGrammar",
				"audioFileDataSource");
	}
	public Aligner(String config, String audioFile, String textFile,
			String recognizerName, String grammarName,
			String audioDataSourceName) throws IOException {
		this(config, audioFile, textFile, recognizerName, grammarName, "",
				audioDataSourceName);
	}	
	
	public Aligner(String config, String audioFile, String textFile,
			String recognizerName, String grammarName, String grammarType,
			String audioDataSourceName) throws IOException {
		this(config, audioFile, textFile, recognizerName, grammarName, grammarType,
				audioDataSourceName, true);
	}	
	
	public Aligner(String config, String audioFile,
			String textFile, String recognizerName, String grammarName,String grammarType,
			String audioDataSourceName, boolean optimize) throws IOException {
		this.config = config;
		this.audioFile = audioFile;
		this.textFile = textFile;
		this.PROP_RECOGNIZER = recognizerName;
		this.PROP_GRAMMAR = grammarName;
		this.PROP_GRAMMAR_TYPE = grammarType;
		this.PROP_AUDIO_DATA_SOURCE = audioDataSourceName;
		this.optimize = optimize;
		txtInTranscription = readTranscription();
		
		
		cm = new ConfigurationManager(config);
		absoluteBeamWidth = cm.getGlobalProperty("absoluteBeamWidth");
		relativeBeamWidth = cm.getGlobalProperty("relativeBeamWidth");
		outOfGrammarProbability = cm.getGlobalProperty("outOfGrammarProbability");
		phoneInsertionProbability = cm.getGlobalProperty("phoneInsertionProbability");
	}
	
	
	@Override
	public boolean setAudio(String path) {
		
		return false;
	}

	@Override
	public boolean setText(String text) throws Exception {
		grammar.setText(text);
		return true;
	}
	
	
	// Idea is to automate the process of selection and setting of 
	// Global properties for alignment giving hands free experience 
	// to first time users.
	@Override
	public void optimize() {
				
	}
	
	private void setGlobalProperties() {
		cm.setGlobalProperty("absoluteBeamWidth", absoluteBeamWidth);
		cm.setGlobalProperty("relativeBeamWidth", relativeBeamWidth);
		cm.setGlobalProperty("outOfGrammarProbability", outOfGrammarProbability);
		cm.setGlobalProperty("phoneInsertionProbability", phoneInsertionProbability);
	}
	
	@Override
	public String align() throws Exception {
		cm = new ConfigurationManager(config);
		optimize();
		setGlobalProperties();
		recognizer = (Recognizer) cm.lookup(PROP_RECOGNIZER);
		grammar = (AlignerGrammar) cm.lookup(PROP_GRAMMAR);
		datasource = (AudioFileDataSource) 
				cm.lookup(PROP_AUDIO_DATA_SOURCE);
		datasource.setAudioFile(new File(audioFile), null);
		allocate();			
		return start_align();
	}
	
	
	private void allocate() throws IOException {
		datasource.setAudioFile(new URL("file:" + audioFile), null);
		grammar.setText(txtInTranscription);
		grammar.setGrammarType(PROP_GRAMMAR_TYPE);
		
		recognizer.allocate();
	}
	
	public void deallocate() {
		recognizer.deallocate();		
	}

	private String readTranscription() throws IOException {
		BufferedReader txtReader = new BufferedReader(new FileReader(textFile));
		String line;
		String finalText = "";
		while((line = txtReader.readLine()) != null){
			finalText += " " + line;
		}
		StringCustomise sc = new StringCustomise();
		return sc.customise(finalText);
	}

	private String start_align() throws IOException{
		Result result = recognizer.recognize();
		String timedResult = result.getTimedBestResult(false, true);
		deallocate();
		return timedResult;
	}
	
	public void generateError(float wer) throws MalformedURLException{
		StringErrorGenerator seg = new StringErrorGenerator(wer, txtInTranscription);
	}
	
	public void generateError(float ir, float dr, float sr){
		
	}

	@Override
	public boolean setGrammarType(String grammarType) {
		PROP_GRAMMAR_TYPE = grammarType;
		return true;		
	}
	
	@Override
	public void setAbsoluteBeamWidth(String absoluteBeamWidth) {
		this.absoluteBeamWidth = absoluteBeamWidth;
		
	}
	@Override
	public void setRelativeBeamWidth(String relativeBeamWidth) {
		this.relativeBeamWidth = relativeBeamWidth;
		
	}
	@Override
	public void setOutOfGrammarProbability(String outOfGrammarProbability) {
		this.outOfGrammarProbability = outOfGrammarProbability;
		
	}
	@Override
	public void setPhoneInsertionProbability(String phoneInsertionProbability) {
		this.phoneInsertionProbability = phoneInsertionProbability;
		
	}
	@Override
	public void setForwardJumpProbability(double prob) {
		grammar.setForwardJumpProbability(prob);
		
	}
	@Override
	public void setBackwardJumpProbability(double prob) {
		grammar.setBackWardTransitionProbability(prob);
		
	}
	@Override
	public void setSelfLoopProbability(double prob) {
		grammar.setSelfLoopProbability(prob);
		
	}
}
