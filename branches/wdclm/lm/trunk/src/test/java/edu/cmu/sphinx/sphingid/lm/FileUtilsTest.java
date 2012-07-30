package edu.cmu.sphinx.sphingid.lm;

import static org.hamcrest.Matchers.is;
import static org.hamcrest.Matchers.not;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

public class FileUtilsTest {
	public static final File basedir = new File(System.getProperty("basedir",
			"."));
	public static final String newline = System.getProperty("line.separator");

	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	@Test
	public void canCountLines() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "test");
		try {
			assertEquals(
					FileUtils.countLines(testFile, Charset.forName("utf-8")),
					2306);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

	@Test
	public void canChooseNRandomLinesWithSentenceMarkers() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "test");

		File output = null;
		try {
			output = tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader tbr = null;
		try {
			tbr = new BufferedReader(new InputStreamReader(new FileInputStream(
					testFile), Charset.forName("utf-8")));
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		String test = new String(), rand = new String();
		try {
			for (int i = 0; i < 20; i++) {
				test += tbr.readLine();
			}
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			for (int times = 0; times < 20; ++times) {
				FileUtils.chooseNRandomLines(testFile, output, 20,
						Charset.forName("utf-8"));

				BufferedReader rbr = new BufferedReader(new InputStreamReader(
						new FileInputStream(output), Charset.forName("utf-8")));

				rand = new String();

				for (int i = 0; i < 20; ++i) {
					rand += rbr.readLine();
				}

				rbr.close();

				if (!rand.equals(test))
					break;
			}
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(test, not(rand));

		try {
			tbr.close();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

	@Test
	public void canGetFirstNLinesWithSentenceMarkers() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "test");

		File output = null;
		try {
			output = tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(testFile, output, 3,
					Charset.forName("utf-8"));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader br = null;
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					output), Charset.forName("utf-8")));
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(br.readLine(), is("</s>"));
			assertThat(br.readLine(),
					is("<s> debates of the senate ( hansard ) </s>"));
			assertThat(br.readLine(),
					is("<s> 2 nd session , 36 th parliament , </s>"));
			assertThat(br.readLine(), is("<s> volume 138 , issue 42 </s>"));
			assertThat(br.readLine(), is("<s>"));
			assertTrue(br.readLine() == null);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			br.close();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

	@Test
	public void canChooseNRandomLines() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "text-a.dict");

		File output = null;
		try {
			output = tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader tbr = null;
		try {
			tbr = new BufferedReader(new InputStreamReader(new FileInputStream(
					testFile), Charset.forName("utf-8")));
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		String test = new String(), rand = new String();
		try {
			for (int i = 0; i < 20; i++) {
				test += tbr.readLine();
			}
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			for (int times = 0; times < 20; ++times) {
				FileUtils.chooseNRandomLines(testFile, output, 20,
						Charset.forName("utf-8"));

				BufferedReader rbr = new BufferedReader(new InputStreamReader(
						new FileInputStream(output), Charset.forName("utf-8")));

				rand = new String();

				for (int i = 0; i < 20; ++i) {
					rand += rbr.readLine();
				}

				rbr.close();

				if (!rand.equals(test))
					break;
			}
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(test, not(rand));

		try {
			tbr.close();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

	@Test
	public void canGetFirstNLines() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "text-a.dict");

		File output = null;
		try {
			output = tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(testFile, output, 3,
					Charset.forName("utf-8"));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader br = null;
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					output), Charset.forName("utf-8")));
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(br.readLine(), is("dictionary 0 12024"));
			assertThat(br.readLine(), is("the"));
			assertThat(br.readLine(), is("</s>"));
			assertTrue(br.readLine() == null);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			br.close();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

	@Test
	public void canCreateTrainingTestSets() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "text-a.dict");

		File firstFive = null;
		File training = null;
		File test = null;

		try {
			firstFive = tempFolder.newFile();
			training = tempFolder.newFile();
			test = tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(testFile, firstFive, 5,
					Charset.forName("utf-8"));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.createTrainingTestSets(firstFive, training, test, false,
					40, Charset.forName("utf-8"));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader trainingReader = null;
		BufferedReader testReader = null;
		try {
			trainingReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(training), Charset.forName("utf-8")));
			testReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(test), Charset.forName("utf-8")));
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(trainingReader.readLine(), is("dictionary 0 12024"));
			assertThat(trainingReader.readLine(), is("the"));
			assertThat(trainingReader.readLine(), is("</s>"));
			assertTrue(trainingReader.readLine() == null);
			assertThat(testReader.readLine(), is("<s>"));
			assertThat(testReader.readLine(), is(","));
			assertTrue(testReader.readLine() == null);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			trainingReader.close();
			testReader.close();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

	}

	@Test
	public void canCreateTrainingTestSetsWithSentenceMarkers() {
		File testFile = new File(basedir + File.separator + "test-data"
				+ File.separator + "test");

		File firstFive = null;
		File training = null;
		File test = null;

		try {
			firstFive = tempFolder.newFile();
			training = tempFolder.newFile();
			test = tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(testFile, firstFive, 5,
					Charset.forName("utf-8"));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.createTrainingTestSets(firstFive, training, test, false,
					40, Charset.forName("utf-8"));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader trainingReader = null;
		BufferedReader testReader = null;
		try {
			trainingReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(training), Charset.forName("utf-8")));
			testReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(test), Charset.forName("utf-8")));
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(trainingReader.readLine(), is("</s>"));
			assertThat(trainingReader.readLine(),
					is("<s> debates of the senate ( hansard ) </s>"));
			assertThat(trainingReader.readLine(),
					is("<s> 2 nd session , 36 th parliament , </s>"));
			assertThat(trainingReader.readLine(),
					is("<s> volume 138 , issue 42 </s>"));
			assertThat(trainingReader.readLine(), is("<s>"));
			assertTrue(trainingReader.readLine() == null);

			assertThat(testReader.readLine(), is("</s>"));
			assertThat(testReader.readLine(),
					is("<s> tuesday , april 4 , 2000 </s>"));
			assertThat(testReader.readLine(),
					is("<s> the honourable gildas l. molgat , speaker </s>"));
			assertThat(testReader.readLine(), is("<s>"));
			assertTrue(testReader.readLine() == null);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			trainingReader.close();
			testReader.close();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

	}
}
