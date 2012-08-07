import java.util.LinkedList;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Word;


public class Sequence implements Comparable<Sequence>{
	private WordSequence words;
	private float probability;
	private int size;
	private Sequence previous;
	
	public Sequence(WordSequence words, float probability, int sequenceNumber, Sequence previous) {
		this.words = words;
		this.size = sequenceNumber;
		if (previous != null)  {
			this.probability = previous.getProbability() + probability;
		}
		else {
			this.probability = 0;
		}
	}
	
	public int getSize() {
		return this.words.size();
	}
	
	public int getSequenceNumber() {
		return this.size;
	}
	
	public float getProbability() {
		return this.probability;
	}
	
	public Word[] getWords() {
		return this.words.getWords();
	}
	
	public WordSequence getWordSequence() {
		return this.words;
	}
	
	public String toString() {
		return words.toString();
	}
	
	public int compareTo(Sequence s) {
		if (this.probability > s.getProbability()) return -1;
		else if (this.probability == s.getProbability()) return 0;
		return 1;
	}
	
	public Sequence getPrev() {
		return this.previous;
	}
}
