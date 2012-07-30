package edu.cmu.sphinx.sphingid.lm;

import java.util.Comparator;

public class SentencePerplexity {
	private int sentenceNumber;
	private float perplexity;

	/**
	 * Constructs a new instance.
	 */
	public SentencePerplexity(int sentenceNumber, float perplexity) {
		this.sentenceNumber = sentenceNumber;
		this.perplexity = perplexity;
	}

	/**
	 * Gets the sentenceNumber for this instance.
	 * 
	 * @return The sentenceNumber.
	 */
	public int getSentenceNumber() {
		return this.sentenceNumber;
	}

	/**
	 * Gets the perplexity for this instance.
	 * 
	 * @return The perplexity.
	 */
	public float getPerplexity() {
		return this.perplexity;
	}

	public static enum SentenceComparator implements
			Comparator<SentencePerplexity> {
		compareBySentenceNumber() {
			public int compare(SentencePerplexity s1, SentencePerplexity s2) {
				if (s1.getSentenceNumber() > s2.getSentenceNumber())
					return 1;
				else if (s1.getSentenceNumber() < s2.getSentenceNumber())
					return -1;
				else
					return 0;
			}
		},

		compareByPerplexity() {
			public int compare(SentencePerplexity s1, SentencePerplexity s2) {
				if (s1.getPerplexity() > s2.getPerplexity())
					return 1;
				else if (s1.getPerplexity() < s2.getPerplexity())
					return -1;
				else
					return 0;
			}
		};

		public abstract int compare(SentencePerplexity s1, SentencePerplexity s2);
	}
}

