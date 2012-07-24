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
			removeHalf();
		}
	}
	
	private void removeHalf() {
		while (stack.size() > maxSize/2) {
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
}
