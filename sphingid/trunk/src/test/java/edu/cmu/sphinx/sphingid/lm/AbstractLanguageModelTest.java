/**
 * 
 */
package edu.cmu.sphinx.sphingid.lm;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

/**
 * @author emre
 * 
 */
public class AbstractLanguageModelTest {
	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@Test
	public void canComputePerplexity() {
		File lmFile = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a.lm").getFile()); //$NON-NLS-1$ 

		File test = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/test").getFile()); //$NON-NLS-1$ 

		float perplexity = 0F;
		
		LanguageModel lm = null;
		
		try {
			lm = new LanguageModel(lmFile);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	
		try {
			perplexity = lm.computePerplexity(test);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (InterruptedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(perplexity, is(296.83002F));
	}

	@Test
	public void canComputeSentencePerplexities() {
		File giglm = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a.giglm.gz").getFile()); //$NON-NLS-1$ 

		File sentences = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/sentence-test").getFile()); //$NON-NLS-1$ 

		ArrayList<SentencePerplexity> perplexities = null;
		try {
			GiganticLanguageModel glm = new GiganticLanguageModel(giglm);
			perplexities = glm.computeSentencePerplexities(sentences);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (InterruptedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		assertThat(perplexities.get(0).getPerplexity(), is(23.50F));
		assertThat(perplexities.get(1).getPerplexity(), is(345.77002F));
		assertThat(perplexities.get(2).getPerplexity(), is(583.8496F));
		assertThat(perplexities.get(3).getPerplexity(), is(177.72998F));
	}
}
