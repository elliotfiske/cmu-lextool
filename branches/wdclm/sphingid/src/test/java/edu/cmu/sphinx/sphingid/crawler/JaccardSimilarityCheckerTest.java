/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;

import org.junit.Before;
import org.junit.Test;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class JaccardSimilarityCheckerTest {
	private JaccardSimilarityChecker checker;

	@Before
	public void initializeChecker() {
		byte n = 3;
		this.checker = new JaccardSimilarityChecker(n);
	}

	@Test
	public void canDetectEqual() {
		String doc1 = "We are completely equal."; //$NON-NLS-1$
		String doc2 = "We are completely equal."; //$NON-NLS-1$

		assertThat(this.checker.getSimilarity(doc1, doc2), is(1.0F));
	}

	@Test
	public void canDetectNotEqual() {
		String doc1 = "We are completely equal."; //$NON-NLS-1$
		String doc2 = "No, there is nothing in common."; //$NON-NLS-1$

		assertThat(this.checker.getSimilarity(doc1, doc2), is(0.0F));
	}

	@Test
	public void canDetectSimilar() {
		String doc1 = "We are completely equal."; //$NON-NLS-1$
		String doc2 = "Are completely equal? I can say that we are similar in some aspects, but we have some big differences too."; //$NON-NLS-1$

		/*
		 * Also checking for float operations
		 */
		assertThat(this.checker.getSimilarity(doc1, doc2), is(1.0F / 19.0F));
	}

	@Test
	public void canDecideWithThresholdCorrectly() {
		String doc1 = "We are completely equal."; //$NON-NLS-1$
		String doc2 = "Are completely equal? I can say that we are similar in some aspects, but we have some big differences too."; //$NON-NLS-1$

		assertThat(this.checker.isSimilar(doc1, doc2, 0.5F), is(false));

		doc1 = "I can confidently say that we are mostly equal."; //$NON-NLS-1$
		doc2 = "I can confidently say that we are completely equal."; //$NON-NLS-1$

		assertThat(this.checker.isSimilar(doc1, doc2, 0.5F), is(true));
	}

	@Test
	public void canGiveResultsForVerySmallText() {
		String doc1 = "Test1."; //$NON-NLS-1$
		String doc2 = "Test2."; //$NON-NLS-1$

		assertThat(this.checker.getSimilarity(doc1, doc2), is(0.0F));

		doc1 = "Test1."; //$NON-NLS-1$
		doc2 = "."; //$NON-NLS-1$

		assertThat(this.checker.getSimilarity(doc1, doc2), is(0.0F));

		/*
		 * Divide by zero test
		 */
		doc1 = "."; //$NON-NLS-1$
		doc2 = "."; //$NON-NLS-1$

		assertThat(this.checker.getSimilarity(doc1, doc2), is(0.0F));
	}

	@SuppressWarnings("static-method")
	@Test(expected = IllegalArgumentException.class)
	public void canThrowExceptionForIllegalN() {
		JaccardSimilarityChecker checker = new JaccardSimilarityChecker(
				(byte) 0);
		checker.getSimilarity("a", "b"); //$NON-NLS-1$ //$NON-NLS-2$
	}
}
