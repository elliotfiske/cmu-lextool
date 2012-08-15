/**
 * 
 */
package edu.cmu.sphinx.sphingid.commons;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.io.RandomAccessFile;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.io.Input;
import com.esotericsoftware.kryo.io.Output;

import edu.cmu.sphinx.sphingid.lm.IntegerArray;

/**
 * Common utility class for file operations.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class FileUtils {
	private static final Logger logger = LoggerFactory
			.getLogger(FileUtils.class);

	/**
	 * Adds sentence markers to a file.
	 * 
	 * @param file
	 *            file to be processed
	 * @param outputFolder
	 *            path to folder into where the result will be putlm.group()
	 * @return the resulting file, for convenience
	 * @throws IOException
	 */
	public static void addSentenceMarkers(File inputFile, File outputFile,
			Charset encoding) throws IOException {
		String newline = System.getProperty("line.separator"); //$NON-NLS-1$

		BufferedReader reader = new BufferedReader(new InputStreamReader(
				new FileInputStream(inputFile), encoding));
		BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(outputFile), encoding));

		writer.write("</s>" + newline); //$NON-NLS-1$

		String temp = ""; //$NON-NLS-1$

		while ((temp = reader.readLine()) != null) {
			temp = temp.trim();
			if (temp.isEmpty())
				continue;

			writer.write("<s> " + temp + " </s>" + newline); //$NON-NLS-1$ //$NON-NLS-2$
		}

		writer.write("<s>"); //$NON-NLS-1$

		reader.close();
		writer.close();

		/*
		 * String result = sb.toString();
		 * 
		 * return result.replaceAll("([^ ]{80})([^ ]{1,})", "\1");
		 */
	}

	public static void chooseNRandomLines(File input, File output, int n,
			Charset encoding) throws FileNotFoundException, IOException {
		if (!input.exists() || !input.canRead())
			throw new IOException(String.format(
					Messages.getString("FileUtils.10"), //$NON-NLS-1$
					input));

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(input), encoding));

		boolean sentenceMarkers = false;

		String first = br.readLine();

		if (first == null) {
			br.close();
			throw new IOException(String.format(
					Messages.getString("FileUtils.FileIsEmpty"), input)); //$NON-NLS-1$
		}

		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(output), encoding));

		if (first.equals("</s>")) { //$NON-NLS-1$
			sentenceMarkers = true;
			bw.write("</s>" + System.getProperty("line.separator")); //$NON-NLS-1$ //$NON-NLS-2$
		}

		br.close();
		br = new BufferedReader(new InputStreamReader(
				new FileInputStream(input), encoding));

		int numLines = countLines(input, encoding);

		if (n > numLines) {
			bw.close();
			throw new IllegalArgumentException(
					Messages.getString("FileUtils.14")); //$NON-NLS-1$
		}

		IntegerArray intArray = new IntegerArray(numLines);
		for (int i = 0; i < numLines; ++i)
			intArray.put(i, i);

		intArray.shuffle();

		String temp = null;
		if (sentenceMarkers) {
			for (int i = 0; i < numLines; i++) {
				temp = br.readLine();
				if (intArray.get(i) < n && !temp.equals("</s>") //$NON-NLS-1$
						&& !temp.equals("<s>")) //$NON-NLS-1$
					bw.write(temp + System.getProperty("line.separator")); //$NON-NLS-1$

			}
		} else {
			for (int i = 0; i < numLines; i++) {
				if (intArray.get(i) < n)
					bw.write(br.readLine()
							+ System.getProperty("line.separator")); //$NON-NLS-1$
				else
					br.readLine();
			}
		}

		if (sentenceMarkers)
			bw.write("<s>"); //$NON-NLS-1$
		br.close();
		bw.close();
	}

	/**
	 * Combines a set of efficiently files using Java nio.
	 * 
	 * @param files
	 *            the files to be combined
	 * @param concatenatedFilePath
	 *            the name and path of the resulting file, e.g.
	 *            /path/to/file/filename
	 * @return a {@link File} object that point to the resulting file
	 * 
	 * @throws IOException
	 */
	public static File concatenateFiles(ArrayList<File> files,
			String concatenatedFilePath) throws IOException {

		FileOutputStream output = new FileOutputStream(concatenatedFilePath);
		FileChannel out = output.getChannel();

		long position = 0;
		for (int i = 0; i < files.size(); i++) {
			RandomAccessFile randomAccessFile = new RandomAccessFile(
					files.get(i), "r"); //$NON-NLS-1$
			FileChannel in = randomAccessFile.getChannel();
			out.transferFrom(in, position, in.size());
			position += in.size();
			in.close();
			randomAccessFile.close();
		}

		output.close();
		out.close();

		return new File(concatenatedFilePath);
	}

	public static int countLines(File file, Charset encoding)
			throws IOException {
		if (!file.exists() || !file.canRead())
			throw new IOException(String.format(
					Messages.getString("FileUtils.2"), //$NON-NLS-1$
					file));

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(file), encoding));

		int lines;
		for (lines = 0; br.ready(); ++lines)
			br.readLine();

		br.close();

		return lines;
	}

	public static void createTrainingTestSets(File corpus, File trainingSet,
			File testSet, boolean chooseRandomly, int testSetPercentage,
			Charset encoding) throws FileNotFoundException, IOException {
		if (!corpus.exists() || !corpus.canRead())
			throw new IOException(String.format(
					Messages.getString("FileUtils.1"), //$NON-NLS-1$
					corpus));

		if (testSetPercentage < 0 || testSetPercentage > 100)
			throw new IllegalArgumentException(String.format(
					Messages.getString("FileUtils.21"), testSetPercentage)); //$NON-NLS-1$

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(corpus), encoding));
		BufferedWriter trainingWriter = new BufferedWriter(
				new OutputStreamWriter(new FileOutputStream(trainingSet),
						encoding));
		BufferedWriter testWriter = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(testSet), encoding));
		boolean sentenceMarkers = false;

		int numLines = countLines(corpus, encoding);
		int trainingSetLines = (int) Math.floor(numLines
				* (100 - testSetPercentage) / 100);

		String first = br.readLine();

		if (first == null) {
			br.close();
			throw new IOException(String.format(
					Messages.getString("FileUtils.FileIsEmpty"), corpus)); //$NON-NLS-1$
		}

		if (first.equals("</s>")) { //$NON-NLS-1$
			sentenceMarkers = true;
			trainingWriter.write("</s>" + System.getProperty("line.separator")); //$NON-NLS-1$ //$NON-NLS-2$
			testWriter.write("</s>" + System.getProperty("line.separator")); //$NON-NLS-1$ //$NON-NLS-2$
		}

		br.close();
		br = new BufferedReader(new InputStreamReader(new FileInputStream(
				corpus), encoding));

		if (chooseRandomly) {
			IntegerArray intArray = new IntegerArray(numLines);
			for (int i = 0; i < numLines; ++i)
				intArray.put(i, i);

			intArray.shuffle();
			if (sentenceMarkers) {
				for (int i = 0; i < numLines; i++) {
					String temp = br.readLine();
					if (temp == null) {
						br.close();
						throw new IOException(
								Messages.getString("FileUtils.LineNumberReturnsNull")); //$NON-NLS-1$
					}
					if (!temp.equals("</s>") && !temp.equals("<s>")) { //$NON-NLS-1$ //$NON-NLS-2$
						if (intArray.get(i) < trainingSetLines)
							trainingWriter.write(temp
									+ System.getProperty("line.separator")); //$NON-NLS-1$
						else
							testWriter.write(temp
									+ System.getProperty("line.separator")); //$NON-NLS-1$
					}
				}
			} else {
				for (int i = 0; i < numLines; i++) {
					if (intArray.get(i) < trainingSetLines)
						trainingWriter.write(br.readLine()
								+ System.getProperty("line.separator")); //$NON-NLS-1$
					else
						testWriter.write(br.readLine()
								+ System.getProperty("line.separator")); //$NON-NLS-1$
				}
			}
		} else {
			int start = 0;
			if (sentenceMarkers) {
				br.readLine();
				start++;
			}

			for (int i = start; i < trainingSetLines; ++i)
				trainingWriter.write(br.readLine()
						+ System.getProperty("line.separator")); //$NON-NLS-1$

			for (int i = trainingSetLines; i < numLines; ++i)
				testWriter.write(br.readLine()
						+ System.getProperty("line.separator")); //$NON-NLS-1$
		}

		if (sentenceMarkers) {
			trainingWriter.write("<s>"); //$NON-NLS-1$
		}

		br.close();
		trainingWriter.close();
		testWriter.close();
	}

	public static void getFirstNLines(File input, File output, int n,
			Charset encoding) throws IOException {
		if (!input.exists() || !input.canRead())
			throw new IOException(String.format(
					Messages.getString("FileUtils.3"), //$NON-NLS-1$
					input));

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(input), encoding));
		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(output), encoding));

		boolean sentenceMarkers = false;

		String first = br.readLine();

		if (first == null) {
			br.close();
			bw.close();
			throw new IOException(String.format(
					Messages.getString("FileUtils.FileIsEmpty"), input)); //$NON-NLS-1$
		}

		if (first.equals("</s>")) { //$NON-NLS-1$
			sentenceMarkers = true;
			bw.write("</s>" + System.getProperty("line.separator")); //$NON-NLS-1$ //$NON-NLS-2$
		} else {
			br.close();
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					input), encoding));
		}

		for (int i = 0; i < n; i++) {
			if (br.ready())
				bw.write(br.readLine() + System.getProperty("line.separator")); //$NON-NLS-1$
			else {
				logger.warn(Messages.getString("FileUtils.8"), n); //$NON-NLS-1$
				br.close();
				bw.close();
				return;
			}
		}

		if (sentenceMarkers)
			bw.write("<s>"); //$NON-NLS-1$

		br.close();
		bw.close();
	}

	public static void serializeObject(File file, Object object) throws FileNotFoundException {
		file.getParentFile().mkdirs();
		Output output = new Output(new FileOutputStream(file));
		Kryo kryo = new Kryo();
		kryo.writeObject(output, object);
		
		output.close();
	}
	
	public static <T> void serializeSynchronizedList(File file, List<T> list) throws FileNotFoundException {
		file.getParentFile().mkdirs();
		Output output = new Output(new FileOutputStream(file));
		Kryo kryo = new Kryo();
		List<T> clone = new ArrayList<T>(list);
		kryo.writeObject(output, clone);
		output.close();
	}
	
	public static <T> T deserializeObject(File file, Class<T> clazz) throws FileNotFoundException {
		Input input = new Input(new FileInputStream(file));
		Kryo kryo = new Kryo();
		T object =kryo.readObject(input, clazz); 
		input.close();
		return object;
	}

	public static boolean contentEquals(File file1, File file2)
			throws IOException {
		return org.apache.commons.io.FileUtils.contentEquals(file1, file2);
	}
}
