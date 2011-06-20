/*
 * Copyright 1999-2004 Carnegie Mellon University.
 * Portions Copyright 2004 Sun Microsystems, Inc.
 * Portions Copyright 2004 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 *
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.util;

import java.util.Iterator;
import java.util.LinkedList;
import java.util.ListIterator;
import java.util.StringTokenizer;

public class WordErrorCount {
	private LinkedList<Word> reference;	// contains words that are reference transcription
	private LinkedList<Word> allWordReference; // contains all words in reference
										// i.e. including the ones inserted and deleted.
	private LinkedList<Word> hypothesis;
	private LinkedList<Word> alignedList; // contains the final aligned Result
	
	private int totalNumWords;
	private int totalInsertions;
	private int totalDeletionsAndSubs;
	private int artificialInsertions;	// keeps count of the number of words 
										// inserted due to corrupted transcription
	
	private int correctedDeletions;	// keeps count of the number of words
										// deleted due to corrupted transcription
	private int correctedSubstitutions;
	private Double wer;
	
	public WordErrorCount() {
		totalNumWords = 0;
		totalInsertions= 0;
		totalDeletionsAndSubs = 0;
		wer = 0.0;
		artificialInsertions = 0;
		correctedDeletions = 0;
		correctedSubstitutions = 0;
	}
	public WordErrorCount(LinkedList<Word> reference, String hypothesis) {
		this();
		this.allWordReference = reference;
		this.reference = new LinkedList<Word>();
		this.hypothesis = new LinkedList<Word>();
		generateRef();
		setHypothesis(hypothesis);
	}
	
	private void generateRef() {
		Iterator<Word> iter = allWordReference.iterator();
		while(iter.hasNext()) {
			Word nextWord = iter.next();
			
			// if the word was inserted in the transcription by ErrGen
			// we don't want that in the reference
			if(!nextWord.isInserted()) {
				reference.add(nextWord);
			}
		}
	}
	// Converts String hypothesis into a LinkedList making it suitable for comparison
	public void setHypothesis(String hyp) {
		StringTokenizer st = new StringTokenizer(hyp);
		while(st.hasMoreTokens()) {
			String word = st.nextToken();
			if(word.compareTo("")!= 0) {
				//hypothesis.add(word);
				String textPart = word.substring(0,word.indexOf("("));
				String timedPart = word.substring(word.indexOf("(") + 1,
								word.indexOf(")"));
				String startTime = timedPart.substring(0,timedPart.indexOf(","));
				String endTime = timedPart.substring(timedPart.indexOf(",")+1);					
				hypothesis.add(new Word(textPart, Double.valueOf(startTime),
									Double.valueOf(endTime),0.1));
			}
		}
	}
	
	
	// Aligns the reference and hypothesis strings and determines the 
	// number of insertions and deletions made in the hypothesis string.
	
	public void align() {
		
		// Create backtrace lattice
		BackTraceObj[][] lattice = createBackTraceLattice();
		
		// Now use traceback info to add all those words from ref 
		// that match and you are done.
		alignedList = traceBack(lattice);
		
		generateStats();
	}
	
	
	private BackTraceObj[][] createBackTraceLattice() {
		BackTraceObj [][]lattice = new BackTraceObj [reference.size() + 1][hypothesis.size()+1];
		for (int j=0; j< hypothesis.size() +1; j++) {
			for(int i= 0; i< reference.size()+ 1; i++) {
				lattice[i][j]= new BackTraceObj();
			}
		}
		
		// We will make two Forward pass of this lattice and one Trace-back pass
		// First forward pass is to intialise  match index for each BackTraceObj
		// Second pass to initialise longest Match length and traceback info
		
		// PASS 1 : A typical element of the lattice would be (i , j)
		
		for(int j= 1; j< hypothesis.size()+ 1; j++) {
			for(int i = 1; i < reference.size()+ 1; i++) {
				Word currRef =  reference.get(i-1);
				if( currRef.isEqual(hypothesis.get(j-1))) {
					lattice[i][j].match = 1;
				}
			}
		}
		
		// PASS 2 : We now move breadth first on the lattice. 
		// Updating longest match info. 
		// NOTE : if possible we always want to have longest match w/o using
		// currRef word since it can be used later (kind of Greedy approach)
		
		for(int j= 1; j< hypothesis.size()+ 1; j++) {
			for(int i= 1; i < reference.size()+ 1; i++){
				
				// case 1: match = 1 for this lattice object
				if (lattice[i][j].match == 1) {
					if(lattice[i-1][j].longestMatchLength >= 
						lattice[i][j-1].longestMatchLength ) {
						
						lattice[i][j].longestMatchLength = 
							lattice[i-1][j].longestMatchLength + 1;
						lattice[i][j].refUsed = true;
						lattice[i][j].prevRef = i-1;
						lattice[i][j].prefHyp = j;						
					} else {
						if(lattice[i][j-1].refUsed) {
							lattice[i][j].longestMatchLength = 
								lattice[i][j-1].longestMatchLength;
						} else {
							lattice[i][j].longestMatchLength = 
								lattice[i][j-1].longestMatchLength +1;
						}
						
						// No matter if ref was used the traceback info remains 
						// the same
						lattice[i][j].refUsed = true;
						lattice[i][j].prevRef = i;
						lattice[i][j].prefHyp = j-1;
					}					
				} else {
					
					// Case 2: match = 0. A bit more tricky so follow wisely.
					// Our aim is to not use currRef as far as possible
					if(lattice[i-1][j].longestMatchLength >= 
						lattice[i][j-1].longestMatchLength) {
						lattice[i][j].longestMatchLength = 
							lattice[i-1][j].longestMatchLength;
						lattice[i][j].refUsed= false;
						lattice[i][j].prevRef = i-1;
						lattice[i][j].prefHyp = j;
					} else {
						lattice[i][j].longestMatchLength = 
							lattice[i][j-1].longestMatchLength;
						lattice[i][j].refUsed = lattice[i][j-1].refUsed;
						lattice[i][j].prevRef = i;
						lattice[i][j].prefHyp = j-1;
					}
				}
			}
		}
		
		// Now since the lattice is made let's print it for once.
		//printLattice(lattice);
		return lattice;
	}
	
	public LinkedList<Word> traceBack(BackTraceObj [][]lattice) {
		
		// Start from the lower right corner and traceback.
		// Add a word from ref only when there is a change in 
		// LongestMatchlength.
		LinkedList<Word> alignedList= new LinkedList<Word>();
		int lastRefAdded = reference.size()+1;
		int i = reference.size();
		int j = hypothesis.size();
		while( i > 0){
			while ( j > 0) {
				//System.out.println(i+ " " + j);
				int prevRef = lattice[i][j].prevRef;
				int prevHyp = lattice[i][j].prefHyp;
				if( lattice[i][j].longestMatchLength > 
					lattice[prevRef][prevHyp].longestMatchLength) {
					alignedList.add(0,reference.get(i-1));
					
					// update the number of words from ref that have been skipped
					totalDeletionsAndSubs += (lastRefAdded - i - 1);
					lastRefAdded = i;
				}
				i = prevRef;
				j = prevHyp;				
			}
		}
		return alignedList;
	}
	
	// Prints a pretty lattice 
	private void printLattice(BackTraceObj[][] lattice) {
		for(int j=1; j< hypothesis.size()+ 1; j++) {
			for(int i=1; i< reference.size()+ 1; i++) {
				//System.out.print(lattice[i][j].match);
				//System.out.print(lattice[i][j].longestMatchLength);
				System.out.print("("+lattice[i][j].prevRef+", "+
						lattice[i][j].prefHyp+") ");
			}
			System.out.println("");
		}
	}
	
	private void generateStats() {
		ListIterator<Word> iter = alignedList.listIterator();
		totalNumWords = reference.size();
		
		// ref.size + tI - tD = hyp.size
		totalInsertions = hypothesis.size() - reference.size() + totalDeletionsAndSubs;
		int Errors = totalDeletionsAndSubs+ totalInsertions;
		wer = (double)Errors/(double)totalNumWords;
		while(iter.hasNext()) {
			Word currWord = iter.next();
			if(currWord.isDeleted()){
				correctedDeletions ++;
			}else if(currWord.isSubstituted()){				
				correctedSubstitutions ++;
			}
		}		
		// Still have to add something to count for insertions. But it will work out :)
	}
	
	public void printStats() {
		System.out.println("Total number of errors present:"+(totalDeletionsAndSubs+totalInsertions));
		System.out.println("Total Insertions present:"+totalInsertions);
		System.out.println("Total Deletions+Substitutions present:"+ totalDeletionsAndSubs);
		System.out.println("Total Corrected Deletions:"+ correctedDeletions);
		System.out.println("Total Corrected Substitutions:"+correctedSubstitutions);
		System.out.println("WER:"+wer);
	}
	
	public int totalDeletionsAndSubstitutions(){
		return totalDeletionsAndSubs;
	}
	
	public int totalInsertions() {
		return totalInsertions;
	}
	
	public int totalCorrectedDeletions() {
		return correctedDeletions;
	}
	
	public class BackTraceObj {
		public int match;	// 0 if no match, 1 if doesn't match
		public int longestMatchLength; 	
		public boolean refUsed;		// true if currRef was used to 
									// get longest match
		
		// TraceBack info
		public int prevRef;
		public int prefHyp;
		
		
		public BackTraceObj() {
			this.match = 0;
			this.longestMatchLength = 0;
			this.refUsed = false;
		}
	}	
	
}
