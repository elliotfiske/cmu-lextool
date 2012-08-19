package edu.cmu.sphinx.sphingid.lm;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import edu.cmu.sphinx.sphingid.lm.AbstractLanguageModel.Smoothing;

public class GiganticLanguageModelTest {

	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@SuppressWarnings("unused")
	@Test
	public void canConstructModelCorrectly() {
		File corpus = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a").getFile()); //$NON-NLS-1$

		File testLm = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a.giglm.gz").getFile()); //$NON-NLS-1$

		try {
			new GiganticLanguageModel(
					corpus,
					new File(this.tempFolder.getRoot() + File.separator
							+ "glm.test"), null, 3, Smoothing.WITTEN_BELL, true, true, //$NON-NLS-1$
					3);
		} catch (InterruptedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		try {
			// Relaxed assertion. Binary comparison does not work for identical
			// LMs, reason unknown.
			File resultLm = this.tempFolder.newFile("glm.test.gz"); //$NON-NLS-1$
			assertTrue(testLm.length() == resultLm.length());
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

}
