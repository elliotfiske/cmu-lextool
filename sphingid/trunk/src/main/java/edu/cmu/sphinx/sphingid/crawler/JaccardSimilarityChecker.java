/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;
import java.util.HashSet;

import edu.cmu.sphinx.sphingid.lm.NGramExtractor;

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
		HashSet<String> ngramsDoc1 = NGramExtractor.extractNgramSet(doc1, this.n);
		HashSet<String> ngramsDoc2 = NGramExtractor.extractNgramSet(doc2, this.n);

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

	

}
