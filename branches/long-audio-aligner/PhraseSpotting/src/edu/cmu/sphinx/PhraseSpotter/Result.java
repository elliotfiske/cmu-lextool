/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

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
		return phraseText + "(" + startTime + "," + endTime + ")" ;
	}

}
