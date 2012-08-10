package edu.cmu.sphinx.sphingid.lm;

import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import edu.cmu.sphinx.sphingid.commons.FileUtils;
import edu.cmu.sphinx.sphingid.lm.AbstractLanguageModel.Smoothing;

public class LanguageModelTest {

	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@SuppressWarnings("unused")
	@Test
	public void canConstructModelCorrectly() {
		File lmFile = null;
		File corpus = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a").getFile()); //$NON-NLS-1$ 

		File existingLmFile = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a.lm").getFile()); //$NON-NLS-1$
		try {
			lmFile = this.tempFolder.newFile("lm.test"); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		try {
			new LanguageModel(corpus, lmFile, null, 3,
					Smoothing.MODIFIED_SHIFT_BETA, true);
		} catch (InterruptedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertTrue(FileUtils.contentEquals(existingLmFile, lmFile));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}
}
