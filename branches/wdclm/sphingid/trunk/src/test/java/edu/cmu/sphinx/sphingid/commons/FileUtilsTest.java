/**
 * 
 */
package edu.cmu.sphinx.sphingid.commons;

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
import java.util.ArrayList;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.junit.Before;
import org.junit.Rule;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import edu.cmu.sphinx.sphingid.commons.FileUtils;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class FileUtilsTest {
	@Rule
	public TemporaryFolder tempFolder = new TemporaryFolder();

	private File testFile;
	private File dictFile;

	@Before
	public void initFiles() {
		this.testFile = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/test").getFile()); //$NON-NLS-1$ //$NON-NLS-2$

		this.dictFile = new File(getClass().getClassLoader()
				.getResource("languageModelTestData/text-a.dict").getFile()); //$NON-NLS-1$ //$NON-NLS-2$
	}

	@SuppressWarnings("unchecked")
	@Test
	public void canWriteAndReadObjects() {
		ArrayList<String> testList = new ArrayList<String>();

		testList.add("one"); //$NON-NLS-1$
		testList.add("two"); //$NON-NLS-1$
		testList.add("three"); //$NON-NLS-1$

		File tmpFile = null;
		try {
			tmpFile = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		
		try {
			FileUtils.serializeObject(tmpFile, testList);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		
		testList = null;
		
		try {
			testList = FileUtils.deserializeObject(tmpFile, ArrayList.class);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(testList.get(0), is("one")); //$NON-NLS-1$
		assertThat(testList.get(1), is("two")); //$NON-NLS-1$
		assertThat(testList.get(2), is("three")); //$NON-NLS-1$
	}

	/*
	 * BROKEN FOR NOW
	 * 
	 * @Test public void canWriteAndReadObjectsToZipFiles() { String one =
	 * "one"; String two = "two"; String three = "three";
	 * 
	 * File tmpZipFile = null; try { tmpZipFile = tmpFolder.newFile(); } catch
	 * (IOException e) { fail(); }
	 * 
	 * try { FileUtils.writeObjectToZipFile(tmpZipFile, one, one + ".file");
	 * FileUtils.writeObjectToZipFile(tmpZipFile, two, two + ".file");
	 * FileUtils.writeObjectToZipFile(tmpZipFile, three, three + ".file"); }
	 * catch (IOException e) { fail(); }
	 * 
	 * String inOne = null, inTwo = null, inThree = null;
	 * 
	 * try { inThree = (String) FileUtils.readObjectFromZipFile(tmpZipFile,
	 * three + ".file"); inOne = (String)
	 * FileUtils.readObjectFromZipFile(tmpZipFile, one + ".file"); inTwo =
	 * (String) FileUtils.readObjectFromZipFile(tmpZipFile, two + ".file"); }
	 * catch (ClassNotFoundException e) { fail(); } catch (IOException e) {
	 * fail(); }
	 * 
	 * assertThat(inOne, is(one)); assertThat(inTwo, is(two));
	 * assertThat(inThree, is(three)); }
	 */

	@Test
	public void canCountLines() {
		try {
			assertEquals(
					FileUtils.countLines(this.testFile,
							Charset.forName("utf-8")), //$NON-NLS-1$
					2306);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
	}

	@Test
	public void canChooseNRandomLinesWithSentenceMarkers() {
		File output = null;
		try {
			output = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader tbr = null;
		try {
			tbr = new BufferedReader(new InputStreamReader(new FileInputStream(
					this.testFile), Charset.forName("utf-8"))); //$NON-NLS-1$
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
				FileUtils.chooseNRandomLines(this.testFile, output, 20,
						Charset.forName("utf-8")); //$NON-NLS-1$

				BufferedReader rbr = new BufferedReader(new InputStreamReader(
						new FileInputStream(output), Charset.forName("utf-8"))); //$NON-NLS-1$

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
		File output = null;
		try {
			output = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(this.testFile, output, 3,
					Charset.forName("utf-8")); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader br = null;
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					output), Charset.forName("utf-8"))); //$NON-NLS-1$
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(br.readLine(), is("</s>")); //$NON-NLS-1$
			assertThat(br.readLine(),
					is("<s> debates of the senate ( hansard ) </s>")); //$NON-NLS-1$
			assertThat(br.readLine(),
					is("<s> 2 nd session , 36 th parliament , </s>")); //$NON-NLS-1$
			assertThat(br.readLine(), is("<s> volume 138 , issue 42 </s>")); //$NON-NLS-1$
			assertThat(br.readLine(), is("<s>")); //$NON-NLS-1$
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
		File output = null;
		try {
			output = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader tbr = null;
		try {
			tbr = new BufferedReader(new InputStreamReader(new FileInputStream(
					this.dictFile), Charset.forName("utf-8"))); //$NON-NLS-1$
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
				FileUtils.chooseNRandomLines(this.testFile, output, 20,
						Charset.forName("utf-8")); //$NON-NLS-1$

				BufferedReader rbr = new BufferedReader(new InputStreamReader(
						new FileInputStream(output), Charset.forName("utf-8"))); //$NON-NLS-1$

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
		File output = null;
		try {
			output = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(this.dictFile, output, 3,
					Charset.forName("utf-8")); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader br = null;
		try {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					output), Charset.forName("utf-8"))); //$NON-NLS-1$
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(br.readLine(), is("dictionary 0 12024")); //$NON-NLS-1$
			assertThat(br.readLine(), is("the")); //$NON-NLS-1$
			assertThat(br.readLine(), is("</s>")); //$NON-NLS-1$
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
		File firstFive = null;
		File training = null;
		File test = null;

		try {
			firstFive = this.tempFolder.newFile();
			training = this.tempFolder.newFile();
			test = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(this.dictFile, firstFive, 5,
					Charset.forName("utf-8")); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.createTrainingTestSets(firstFive, training, test, false,
					40, Charset.forName("utf-8")); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader trainingReader = null;
		BufferedReader testReader = null;
		try {
			trainingReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(training), Charset.forName("utf-8"))); //$NON-NLS-1$
			testReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(test), Charset.forName("utf-8"))); //$NON-NLS-1$
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(trainingReader.readLine(), is("dictionary 0 12024")); //$NON-NLS-1$
			assertThat(trainingReader.readLine(), is("the")); //$NON-NLS-1$
			assertThat(trainingReader.readLine(), is("</s>")); //$NON-NLS-1$
			assertTrue(trainingReader.readLine() == null);
			assertThat(testReader.readLine(), is("<s>")); //$NON-NLS-1$
			assertThat(testReader.readLine(), is(",")); //$NON-NLS-1$
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
		File firstFive = null;
		File training = null;
		File test = null;

		try {
			firstFive = this.tempFolder.newFile();
			training = this.tempFolder.newFile();
			test = this.tempFolder.newFile();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.getFirstNLines(this.testFile, firstFive, 5,
					Charset.forName("utf-8")); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			FileUtils.createTrainingTestSets(firstFive, training, test, false,
					40, Charset.forName("utf-8")); //$NON-NLS-1$
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader trainingReader = null;
		BufferedReader testReader = null;
		try {
			trainingReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(training), Charset.forName("utf-8"))); //$NON-NLS-1$
			testReader = new BufferedReader(new InputStreamReader(
					new FileInputStream(test), Charset.forName("utf-8"))); //$NON-NLS-1$
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			assertThat(trainingReader.readLine(), is("</s>")); //$NON-NLS-1$
			assertThat(trainingReader.readLine(),
					is("<s> debates of the senate ( hansard ) </s>")); //$NON-NLS-1$
			assertThat(trainingReader.readLine(),
					is("<s> 2 nd session , 36 th parliament , </s>")); //$NON-NLS-1$
			assertThat(trainingReader.readLine(),
					is("<s> volume 138 , issue 42 </s>")); //$NON-NLS-1$
			assertThat(trainingReader.readLine(), is("<s>")); //$NON-NLS-1$
			assertTrue(trainingReader.readLine() == null);

			assertThat(testReader.readLine(), is("</s>")); //$NON-NLS-1$
			assertThat(testReader.readLine(),
					is("<s> tuesday , april 4 , 2000 </s>")); //$NON-NLS-1$
			assertThat(testReader.readLine(),
					is("<s> the honourable gildas l. molgat , speaker </s>")); //$NON-NLS-1$
			assertThat(testReader.readLine(), is("<s>")); //$NON-NLS-1$
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
