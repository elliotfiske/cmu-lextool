import java.util.LinkedList;

import edu.cmu.sphinx.linguist.WordSequence;
import edu.cmu.sphinx.linguist.dictionary.Word;


public class Sequence implements Comparable<Sequence>{
	private WordSequence words;
	private float probability;
	private int sequenceNumber;
	
	public Sequence(WordSequence words, float probability, int sequenceNumber) {
		this.words = words;
		this.probability = probability;
		this.sequenceNumber = sequenceNumber;
	}
	
	public int getSequenceNumber() {
		return this.sequenceNumber;
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
}
