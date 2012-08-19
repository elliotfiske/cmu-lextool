/**
 * 
 */
package edu.cmu.sphinx.sphingid.lm;

import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.TreeMap;

import edu.cmu.sphinx.sphingid.crawler.Messages;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class NGramExtractor {
	public static HashSet<String> extractNgramSet(String document, byte n) {
		if (n < 1)
			throw new IllegalArgumentException(
					Messages.getString("JaccardSimilarityChecker.0")); //$NON-NLS-1$

		String[] words = document.split("([.,!?:;'\"-]|\\s)+"); //$NON-NLS-1$

		int numNgrams = Math.max(words.length - n + 1, 1);

		HashSet<String> ngrams = new HashSet<String>(numNgrams);

		if (words.length == 0)
			return ngrams;

		if (numNgrams == 1) {
			String ngram = words[0];
			for (int i = 1; i < words.length; i++)
				ngram += " " + words[i]; //$NON-NLS-1$

			ngrams.add(ngram.toLowerCase());
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

	public static TreeMap<String, Integer> extractNgramsWithCounts(
			String document, byte n) {
		if (n < 1)
			throw new IllegalArgumentException(
					Messages.getString("JaccardSimilarityChecker.0")); //$NON-NLS-1$

		String[] words = document.split("([.,!?:;'\"-]|\\s)+"); //$NON-NLS-1$

		int numNgrams = Math.max(words.length - n + 1, 1);

		HashMap<String, Integer> ngrams = new HashMap<String, Integer>();

		if (words.length == 0)
			return new TreeMap<String, Integer>();
		if (numNgrams == 1) {
			String ngram = words[0];
			for (int i = 1; i < words.length; i++)
				ngram += " " + words[i]; //$NON-NLS-1$

			ngrams.put(ngram.toLowerCase(), 1);
		} else {
			for (int i = 0; i < numNgrams; i++) {
				String ngram = words[i];
				for (int j = 1; j < n; j++)
					ngram += " " + words[i + j]; //$NON-NLS-1$

				if (ngrams.containsKey(ngram)) {
					ngrams.put(ngram.toLowerCase(), ngrams.get(ngram) + 1);
				} else {
					ngrams.put(ngram.toLowerCase(), 1);
				}
			}
		}

		TreeMap<String, Integer> result = new TreeMap<String, Integer>(
				new NGramExtractor().new FrequencyComparator(ngrams));
		result.putAll(ngrams);
		return result;
	}

	private class FrequencyComparator implements Comparator<String> {
		Map<String, Integer> valueMap;

		public FrequencyComparator(Map<String, Integer> valueMap) {
			this.valueMap = valueMap;
		}

		@Override
		public int compare(String o1, String o2) {
			return -1 * this.valueMap.get(o1).compareTo(this.valueMap.get(o2));
		}
	}
}
