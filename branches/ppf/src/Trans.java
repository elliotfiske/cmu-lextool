/**
 * Trans(itions) are the edges in the hyperstring
 * @author Alexandru Tomescu
 *
 */
public class Trans {
	private State start;
	private State finish;
	
	private String value;
	
	public Trans (State start, State finish, String value) {
		this.start = start;
		this.finish = finish;
		this.value = value;
	}
	
	void setStart(State start) {
		this.start = start;
	}
	
	State getStart() {
		return this.start;
	}
	
	void setFinish(State finish) {
		this.finish = finish;
	}
	
	State getFinish() {
		return this.finish;
	}
	
	void setValue(String value) {
		this.value = value;
	}
	
	String getValue() {
		return this.value;
	}
}
