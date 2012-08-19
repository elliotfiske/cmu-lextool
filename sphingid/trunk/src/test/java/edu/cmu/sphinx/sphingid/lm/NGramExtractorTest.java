/**
 * 
 */
package edu.cmu.sphinx.sphingid.lm;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;

import java.util.Map;
import java.util.TreeMap;

import org.junit.Test;

/**
 * @author emre
 * 
 */
public class NGramExtractorTest {
	@SuppressWarnings("static-method")
	@Test
	public void canExtractNgramsWithCounts() {
		String test = "Lorem ipsum dolor sit amet, lorem ipsum dolor."; //$NON-NLS-1$

		TreeMap<String, Integer> results = NGramExtractor
				.extractNgramsWithCounts(test, (byte) 3);
		assertThat(results.get("lorem ipsum dolor"), is(2)); //$NON-NLS-1$
		assertThat(results.get("ipsum dolor sit"), is(1)); //$NON-NLS-1$
		assertThat(results.get("dolor sit amet"), is(1)); //$NON-NLS-1$
		assertThat(results.get("sit amet lorem"), is(1)); //$NON-NLS-1$
		assertThat(results.get("amet lorem ipsum"), is(1)); //$NON-NLS-1$
	}

	@SuppressWarnings("static-method")
	@Test
	public void canExtractSorted() {
		String test = "Lorem ipsum dolor sit amet, dolor sit amet."; //$NON-NLS-1$
		TreeMap<String, Integer> results = NGramExtractor
				.extractNgramsWithCounts(test, (byte) 3);
		Map.Entry<String, Integer> entry = results.pollFirstEntry();
		assertThat(entry.getKey(), is("dolor sit amet")); //$NON-NLS-1$
		assertThat(entry.getValue(), is(2));

	}

}
