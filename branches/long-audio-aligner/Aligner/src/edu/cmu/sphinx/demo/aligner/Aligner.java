package edu.cmu.sphinx.demo.aligner;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.net.URL;

import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.linguist.aflat.AFlatLinguist;
import edu.cmu.sphinx.linguist.language.grammar.AlignerGrammar;
import edu.cmu.sphinx.recognizer.Recognizer;
import edu.cmu.sphinx.result.Result;
import edu.cmu.sphinx.util.StringCustomise;
import edu.cmu.sphinx.util.props.ConfigurationManager;

public class Aligner implements AudioAlignerInterface{
	
	private String PROP_GRAMMAR;	// which grammar to use from config
	private String PROP_RECOGNIZER;	// which recognizer to use from config
	private String PROP_GRAMMAR_TYPE;
	private String PROP_AUDIO_DATA_SOURCE;
	
	private ConfigurationManager cm;
	private Recognizer recognizer;
	private AlignerGrammar grammar;
	
	
	private boolean optimize;	// by default set this false
	
	private String config;
	private String audioFile;
	private String textFile;
	
	public Aligner(String config, String audioFile, String textFile,
			String recognizerName, String grammarName, String audioDataSourceName) {
		this(config, audioFile, textFile, recognizerName, grammarName, "", audioDataSourceName);
	}	
	
	public Aligner(String config, String audioFile, String textFile,
			String recognizerName, String grammarName, String grammarType,
			String audioDataSourceName) {
		this(config, audioFile, textFile, recognizerName, grammarName, grammarType,
				audioDataSourceName, false);
	}	
	
	public Aligner(String config, String audioFile,
			String textFile, String recognizerName, String grammarName,
			String grammarType, String audioDataSourceName, boolean optimize ){
		this.config = config;
		this.audioFile = audioFile;
		this.textFile = textFile;
		this.PROP_RECOGNIZER = recognizerName;
		this.PROP_GRAMMAR = grammarName;
		this.PROP_GRAMMAR_TYPE = grammarType;
		this.PROP_AUDIO_DATA_SOURCE = audioDataSourceName;
		this.optimize = optimize;
	}
	
	
	@Override
	public boolean setAudio(String path) {
		
		return false;
	}

	@Override
	public boolean setText(String path) {
		
		return false;
	}

	@Override
	public void optimize() {
		
	}
	
	
	@Override
	public String align() throws Exception {
		optimize();
		allocate();			
		return start_align();
	}
	
	
	private void allocate() throws IOException {
		cm = new ConfigurationManager(config);
		recognizer = (Recognizer) cm.lookup(PROP_RECOGNIZER);
		grammar = (AlignerGrammar) cm.lookup(PROP_GRAMMAR);
		AudioFileDataSource datasource = (AudioFileDataSource) 
				cm.lookup(PROP_AUDIO_DATA_SOURCE);
		datasource.setAudioFile(new File(audioFile), null);
		String txtInTranscription = readTranscription();
		grammar.setText(txtInTranscription);
		grammar.setGrammarType(PROP_GRAMMAR_TYPE);
		recognizer.allocate();
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
		Result result;
		String timedResult;
		result = recognizer.recognize();
		timedResult = result.getTimedBestResult(false, true); // Base result
		return timedResult;
	}


	@Override
	public boolean newGrammarType(String grammarType) {
		PROP_GRAMMAR_TYPE = grammarType;
		recognizer.deallocate();
		grammar.setGrammarType(PROP_GRAMMAR_TYPE);
		recognizer.allocate();
		return true;		
	}
}
