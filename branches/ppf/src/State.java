import java.util.LinkedList;

/**
 * State objects are the nodes in the hyperstring
 * @author Alexandru Tomescu
 *
 */
public class State {
	public enum Type {
		s, t
	}
	
	private int seq;
	private Type type;
	private State nextState, previousState;
	private LinkedList<Trans> transitions;
	
	public State(int seq, Type type) {
		this.seq = seq;
		this.type = type;
		transitions = new LinkedList<Trans>();
		nextState = null;
		previousState = null;
	}
	
	public void addTransition(Trans t) {
		transitions.add(t);
	}
	
	public LinkedList<Trans> getTransitions() {
		return this.transitions;
	}
	
	public void setNext(State s) {
		this.nextState = s;
	}
	
	public State getNext() {
		return this.nextState;
	}
	
	public void setPrev(State s) {
		this.previousState = s;
	}
	
	public State getPrev() {
		return this.previousState;
	}
	
	public void setType(Type t) {
		this.type = t;
	}
	
	public Type getType() {
		return this.type;
	}
	
	public void setSeq(int s) {
		this.seq = s;
	}
	
	public int getSeq() {
		return this.seq;
	}
	
	public String toString() {
		return this.type + "" + this.seq;
	}
	
}
