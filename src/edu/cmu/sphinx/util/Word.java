package edu.cmu.sphinx.util;

public class Word {
	private  String word;
	private double startTime;
	private double endTime;
	private double tolerance;
	private String FLAG; // contains information of whether the word is inserted
						 // or deleted. 
	
	
	public Word () {
		this(null,"",0.0, 0.0,0.0);
	}
	public Word(String word) {
		this (word,"",0.0, 0.0, 0.0);
	}
	public Word (String word, double startTime, double endTime, double tolerance) {
		this(word,"", startTime, endTime, tolerance);
	}
	public Word(String word, String FLAG, double startTime, double endTime, double tolerance ) {
		this.word = word;
		this.FLAG = FLAG;
		this.startTime = startTime;
		this.endTime = endTime;
		this.tolerance = tolerance;
		//System.out.println(word+"("+startTime+","+endTime+") ");
	}
	
	public void insert() {
		//System.out.println("inserting:"+word);
		setFlag("insert");
	}
	
	public void delete() {
		//System.out.println("deleting:"+word);
		setFlag("delete");
	}
	
	public void substitute() {
		setFlag("del+substitute");
	}
	public void substituteWord() {
		setFlag("ins+substitute");
	}
	//get functions
	public String getWord() {
		return word;
	}
	public String getFlag() {
		return FLAG;
	}
	
	//set functions
	public void setFlag(String flag) {
		this.FLAG= flag;
	}
	
	public boolean isInserted() {
		if(FLAG.compareTo("insert")==0) {
			return true;
		}else
			return false;
	}
	public boolean isDeleted() {
		if(FLAG.compareTo("delete")== 0) {
			return true;
		} else {
			return false;
		}
	}
	public boolean isSubstituted() {
		if(FLAG.compareToIgnoreCase("del+substitute")==0){
			return true;
		}else {
			return false;
		}
	}
	public boolean isAddedAsSubstitute() {
		if(FLAG.compareTo("ins+substitute")== 0){
			return true;
		} else {
			return false;
		}
	}
	
	private double getStartTime() {
		return startTime;
	}
	
	private double getEndTime() {
		return endTime;
	}
	
	public boolean isEqual(Word e) {
		if ( e.getWord().compareToIgnoreCase(this.getWord())== 0 &&
				Math.abs(e.getStartTime() - this.getStartTime())<=tolerance &&
				Math.abs(e.getEndTime()-this.getEndTime())<=tolerance) {
			return true;
		} else {
			return false;
		}
	}
	public boolean isEqualNoTolerance(Word e) {
		if(e.getWord().compareToIgnoreCase(this.getWord())== 0){
			return true;
		} else {
			return false;
		}
	}
}
