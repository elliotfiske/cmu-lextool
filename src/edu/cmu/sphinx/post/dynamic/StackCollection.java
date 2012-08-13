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
import java.util.Iterator;
import java.util.LinkedList;

import edu.cmu.sphinx.post.dynamic.SequenceStack;


public class StackCollection {
	LinkedList<SequenceStack> stacks;
	int size;
	int stackSize;
	int currentStack;
	
	public StackCollection(int stackSize) {
		this.stacks = new LinkedList<SequenceStack>();
		this.stackSize = stackSize;
		this.size = 0;
		this.currentStack = 0;
	}
	
	public void addSequence(Sequence s) {
		if (size <= s.getSize()) {
			for (int i = size; i <= s.getSize(); i++) {
				stacks.add(new SequenceStack(stackSize));
			}
			size = s.getSize();
		}
		stacks.get(s.getSize()).addSequence(s);
	}
	
	public boolean isEmpty() {
		Iterator<SequenceStack> it = stacks.iterator();
		while (it.hasNext()) {
			if (!it.next().isEmpty()) return false;
		}
		return true;
	}
	
	public Sequence getSequence() {
		Iterator<SequenceStack> it = stacks.iterator();
		while (it.hasNext()) {
			SequenceStack s = it.next();
			if (!s.isEmpty()) {
				return s.getSequence();
			}
		}
		return null;
	}
	
	public String toString() {
		String str = "";
		int i = 0;
		Iterator<SequenceStack> it = stacks.iterator();
		while (it.hasNext()) {
			SequenceStack s = it.next();
			if (!s.isEmpty()) {
				str += i + " " +s.toString();
			}
			i++;
		}
		return str;
	}
}
