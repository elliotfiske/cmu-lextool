package edu.cmu.sphinx.sphingid.lm;

import static org.junit.Assert.*;
import java.util.ArrayList;
import java.util.Collections;

import org.junit.Test;

public class SentencePerplexityTest {

	@Test
	public void canCompare() {
		SentencePerplexity sentencePerplexity1 = new SentencePerplexity(1,
				(float) 500.0);
		SentencePerplexity sentencePerplexity2 = new SentencePerplexity(2,
				(float) 250.0);

		ArrayList<SentencePerplexity> sentencePerplexities = new ArrayList<SentencePerplexity>();

		sentencePerplexities.add(sentencePerplexity1);
		sentencePerplexities.add(sentencePerplexity2);

		Collections.sort(sentencePerplexities,
				SentencePerplexity.SentenceComparator.compareByPerplexity);
		assertEquals(sentencePerplexities.get(0), sentencePerplexity2);

		Collections.sort(sentencePerplexities,
				SentencePerplexity.SentenceComparator.compareBySentenceNumber);
		assertEquals(sentencePerplexities.get(0), sentencePerplexity1);
	}
}
