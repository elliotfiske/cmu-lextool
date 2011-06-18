package edu.cmu.sphinx.util;

public class Word {
	private  String word;
	private String FLAG; // contains information of whether the word is inserted
						 // or deleted. 
	
	public Word () {
		this(null,"");
	}
	public Word(String word) {
		this (word,"");
	}
	public Word(String word, String FLAG) {
		this.word = word;
		this.FLAG = FLAG;
	}
	
	public void insert() {
		//System.out.println("inserting:"+word);
		setFlag("insert");
	}
	
	public void delete() {
		//System.out.println("deleting:"+word);
		setFlag("delete");
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
}
