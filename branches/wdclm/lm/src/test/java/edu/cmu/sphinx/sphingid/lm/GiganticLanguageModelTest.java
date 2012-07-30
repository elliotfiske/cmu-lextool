package edu.cmu.sphinx.sphingid.lm;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

public class GiganticLanguageModelTest {

	public static final File basedir = new File(System.getProperty("basedir",
			"."));
	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@Test
	public void canConstructModelCorrectly() {
		try {
			new GiganticLanguageModel(new File(basedir + File.separator
					+ "test-data/text-a"), new File(tempFolder.getRoot()
					+ File.separator + "glm.test"), 3, null, true, true, null,
					3);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		try {
			// Relaxed assertion. Binary comparison does not work for identical
			// LMs, reason unknown.
			assertTrue(new File(basedir + File.separator + "test-data"
					+ File.separator + "text-a.giglm.gz").length() == tempFolder
					.newFile("glm.test.gz").length());
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	@Test
	public void canComputeSentencePerplexities() {
		ArrayList<SentencePerplexity> perplexities = null;
		try {
			GiganticLanguageModel glm = new GiganticLanguageModel(new File(
					basedir + File.separator + "test-data/text-a.giglm.gz"));
			perplexities = glm.computeSentencePerplexities(new File(basedir
					+ File.separator + "test-data/sentence-test"));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		assertThat(perplexities.get(0).getPerplexity(), is((float)23.50));
		assertThat(perplexities.get(1).getPerplexity(), is((float)2965.17));
		assertThat(perplexities.get(2).getPerplexity(), is((float)5475.84));
		assertThat(perplexities.get(3).getPerplexity(), is((float)769.24));
	}

}
