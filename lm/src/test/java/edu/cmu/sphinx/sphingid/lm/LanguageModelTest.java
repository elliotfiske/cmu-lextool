package edu.cmu.sphinx.sphingid.lm;

import java.io.File;
import java.io.IOException;

import static org.junit.Assert.*;
import org.junit.Rule;

import org.junit.rules.TemporaryFolder;

import org.junit.Test;

public class LanguageModelTest {

	public static final File basedir = new File(System.getProperty("basedir",
			"."));

	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@Test
	public void canConstructModelCorrectly() {
		File lmFile = null;

		try {
			lmFile = tempFolder.newFile("lm.test");
		} catch (IOException e1) {
			// TODO Auto-generated catch block
			e1.printStackTrace();
		}
		try {
			new LanguageModel(new File(basedir
					+ File.separator + "test-data/text-a"), lmFile, 3, "msb",
					true);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}

		try {
			assertTrue(FileUtils.contentEquals(new File(basedir
					+ File.separator + "test-data/text-a.lm"), lmFile));
		} catch (IOException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}
}
