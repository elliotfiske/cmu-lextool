package edu.cmu.sphinx.PhraseSpotter;


/**
 * 
 * @author apurv
 * This class kind of models a typical instance of spotted phrase in an utterance.
 * Contains the start and the end time of the entire phrase.
 * 
 */
public class Result {
	private String phraseText;
	private float startTime;
	private float endTime;
		
	public Result() {
		
	}
	
	public Result(String phraseText, float startTime, float endTime) {
		this.phraseText = phraseText;
		this.startTime = startTime;
		this.endTime = endTime;
	}
	
	public float getStartTime(){
		return startTime;
	}
	
	public float getEndTime(){
		return endTime;
	}
	
	public void setStartTime(float time){
		startTime = time;
	}
	
	public void setEndTime(float time) {
		endTime = time;
	}
	
	@Override
	public String toString(){
		return phraseText + " " + "<" + startTime + "," + endTime + ">" ;
	}

}
