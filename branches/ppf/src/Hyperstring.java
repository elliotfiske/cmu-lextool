import java.util.Iterator;
import java.util.LinkedList;
import org.apache.commons.lang.WordUtils;

/**
 * Create a hyperstring containing words and puctuation
 * @author Alexandru Tomescu
 *
 */
public class Hyperstring {
	private String sentence;
	private LinkedList<State> states;
	private LinkedList<String> punctuation;
	private int current;
	private boolean process_punctuation;
	/**
	 * 
	 * @param sentence
	 * @param process_punctuation - if true, punctuation will be processed
	 */
	public Hyperstring(String sentence, boolean process_punctuation) {
		this.sentence = sentence;
		this.current = 0;
		this.process_punctuation = process_punctuation;
		this.punctuation = new LinkedList<String>();
		this.punctuation.add("<COMMA>");
		this.punctuation.add("<PERIOD>");
		this.punctuation.add("<NONE>");
		states = new LinkedList<State>();
		states.add(new State(0, State.Type.s));
				
		String[] words = parseSentence();
		for (String word : words) {
			LinkedList<String> word_form = new LinkedList<String>();
			word_form.add(word.toLowerCase());
			word_form.add(WordUtils.capitalize(word));
			word_form.add(word.toUpperCase());
			this.addTransitions(word_form);
			if (process_punctuation) {
				this.addTransitions(punctuation);
			}
		}
	}
	
	String[] parseSentence() {
		String[] words = this.sentence.split(" "); 
		return words;
	}

	
	void addTransitions(LinkedList<String> word) { 
		
		State current_state = states.getLast();
		State st = new State(current, (this.process_punctuation) ? 
				((this.states.getLast().getType() == State.Type.s) ? 
						State.Type.t : State.Type.s) : State.Type.s);
		
		Iterator<String> it = word.iterator();
		while(it.hasNext()) {
			Trans t = new Trans(current_state, st, it.next());
			current_state.addTransition(t);
		}
		st.setPrev(current_state);
		current_state.setNext(st);
		states.add(st);	
		if (process_punctuation && st.getType() == State.Type.t)
			current++;
		
		if (!process_punctuation)
			current++;
	}
	
	public String toString() {
		State current_state = states.getFirst();
		String s = "";

		while(current_state.getNext() != null) {
			s += current_state.toString() + "--(";
			LinkedList<Trans> transitions = current_state.getTransitions();
			Iterator<Trans> it = transitions.iterator();
			while (it.hasNext()) {
				s += it.next().getValue() + " ";
			}
			s += ")-->";
			current_state = current_state.getNext();
		}
		
		return s;
	}
	
	
}
