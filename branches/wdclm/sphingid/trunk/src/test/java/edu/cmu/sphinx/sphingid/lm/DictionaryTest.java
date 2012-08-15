/**
 * 
 */
package edu.cmu.sphinx.sphingid.lm;

import static org.hamcrest.CoreMatchers.is;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import edu.cmu.sphinx.sphingid.commons.FileUtils;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class DictionaryTest {
	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@SuppressWarnings("unused")
	@Test
	public void canExtractDictionary() {
		File text = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a").getFile()); //$NON-NLS-1$ 
		File dictionary = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a.dict").getFile()); //$NON-NLS-1$

		File newDictionary = null;
		try {
			newDictionary = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		try {
			new Dictionary(text, newDictionary, 2, -1);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (InterruptedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		try {
			assertThat(FileUtils.contentEquals(newDictionary, dictionary),
					is(true));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

	}

}
