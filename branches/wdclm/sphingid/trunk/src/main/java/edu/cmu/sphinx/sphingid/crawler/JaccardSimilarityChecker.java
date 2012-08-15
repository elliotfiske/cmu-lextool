/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;
import java.util.HashSet;

/**
 * A simple document similarity checker for comparing two documents. Uses
 * Jaccard similarity.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class JaccardSimilarityChecker implements DocumentSimilarityChecker {
	private byte n;

	public JaccardSimilarityChecker(byte n) {
		this.n = n;
	}
	
	private JaccardSimilarityChecker () {
		super();
	}

	public boolean isSimilar(String doc1, String doc2, float similarityThreshold) {
		return getSimilarity(doc1, doc2) > similarityThreshold;
	}

	public float getSimilarity(String doc1, String doc2) {
		HashSet<String> ngramsDoc1 = extractNgrams(doc1, this.n);
		HashSet<String> ngramsDoc2 = extractNgrams(doc2, this.n);

		HashSet<String> union = new HashSet<String>(ngramsDoc1.size()
				+ ngramsDoc2.size());
		union.addAll(ngramsDoc1);
		union.addAll(ngramsDoc2);

		/*
		 * Prevent divide by zero
		 */
		if (union.size() == 0)
			return 0;

		HashSet<String> intersection = new HashSet<String>(Math.min(
				ngramsDoc1.size(), ngramsDoc2.size()));
		intersection.addAll(ngramsDoc1);
		intersection.retainAll(ngramsDoc2);

		float jaccard = (float) (intersection.size()) / union.size();

		return jaccard;
	}

	static HashSet<String> extractNgrams(String doc1, byte n) {
		if (n < 1)
			throw new IllegalArgumentException(
					Messages.getString("JaccardSimilarityChecker.0")); //$NON-NLS-1$

		String[] words = doc1.split("([.,!?:;'\"-]|\\s)+"); //$NON-NLS-1$

		int numNgrams = Math.max(words.length - n + 1, 1);

		HashSet<String> ngrams = new HashSet<String>(numNgrams);

		if (words.length == 0)
			return ngrams;

		if (numNgrams == 1) {
			String ngram = words[0];
			for (int i = 1; i < words.length; i++)
				ngram += " " + words[i]; //$NON-NLS-1$

			ngrams.add(ngram);
		} else {
			for (int i = 0; i < numNgrams; i++) {
				String ngram = words[i];
				for (int j = 1; j < n; j++)
					ngram += " " + words[i + j]; //$NON-NLS-1$

				ngrams.add(ngram.toLowerCase());
			}
		}
		return ngrams;
	}

}
