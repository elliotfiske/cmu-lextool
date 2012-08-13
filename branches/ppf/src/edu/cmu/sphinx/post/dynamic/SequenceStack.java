/*                                                                              
 * 
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

package edu.cmu.sphinx.post.dynamic;
import java.util.Collections;
import java.util.LinkedList;


public class SequenceStack {
	private LinkedList<Sequence> stack;
	private int maxSize;
	
	public SequenceStack(int size) {
		stack = new LinkedList<Sequence>();
		maxSize = size;
	}
	
	public void addSequence(Sequence s) {
		stack.add(s);
		
		if (stack.size() == maxSize) {
			Collections.sort(stack);
			discardElements();
		}
	}
	
	private void discardElements() {
		while (stack.size() > maxSize/5) {
			stack.pollLast();
		}
	}
	
	public Sequence getSequence() {
		return stack.poll();
	}
	
	public boolean isEmpty() {
		if (stack.isEmpty()) return true;
		return false;
	}
	
	public int getSize() {
		return this.stack.size();
	}
	
	public String toString() {
		return stack.toString();
	}
}
