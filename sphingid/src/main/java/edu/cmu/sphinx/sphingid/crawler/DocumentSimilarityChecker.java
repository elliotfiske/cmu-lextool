package edu.cmu.sphinx.sphingid.crawler;

/**
 * Interface for document similarity checking algorithms.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public interface DocumentSimilarityChecker {
	/**
	 * Returns true if two documents are similar for given threshold.
	 * 
	 * @param doc1
	 *            a text document
	 * @param doc2
	 *            a text document
	 * @param similarityThreshold
	 *            threshold which will be used for determining similarity
	 * @return true if documents are similar
	 */
	public boolean isSimilar(String doc1, String doc2, float similarityThreshold);

	/**
	 * Returns similarity amount of two documents
	 * 
	 * @param doc1
	 *            a text document
	 * @param doc2
	 *            a text document
	 * @return similarity amount
	 */
	public float getSimilarity(String doc1, String doc2);
}
