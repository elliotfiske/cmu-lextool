import java.util.Iterator;
import java.util.LinkedList;


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
		if (size < s.getSize()) {
			for (int i = size; i < s.getSize(); i++) {
				stacks.add(new SequenceStack(stackSize));
			}
			size = s.getSize();
		}
		stacks.get(s.getSize() - 1).addSequence(s);
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
