package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.io.RandomAccessFile;

import java.nio.channels.FileChannel;

import java.nio.charset.Charset;

import java.util.ArrayList;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class FileUtils extends org.apache.commons.io.FileUtils {
	private static final Logger logger = LoggerFactory
			.getLogger(FileUtils.class);

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
					files.get(i), "r");
			FileChannel in = randomAccessFile.getChannel();
			out.transferFrom(in, position, in.size());
			position += in.size();
			in.close();
		}

		out.close();

		return new File(concatenatedFilePath);
	}

	public static int countLines(File file, Charset encoding)
			throws IOException {
		if (!file.exists() || !file.canRead())
			throw new IOException(
					"Cannot access file "
							+ file
							+ ". Check if the file exists or permissions have been set correctly.");

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(file), encoding));

		int lines;
		for (lines = 0; br.ready(); ++lines)
			br.readLine();

		br.close();

		return lines;
	}

	public static void getFirstNLines(File input, File output, int n,
			Charset encoding) throws IOException {
		if (!input.exists() || !input.canRead())
			throw new IOException(
					"Cannot access file "
							+ input
							+ ". Check if the file exists or permissions have been set correctly.");

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(input), encoding));
		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(output), encoding));

		boolean sentenceMarkers = false;
		if (br.readLine().equals("</s>")) {
			sentenceMarkers = true;
			bw.write("</s>" + System.getProperty("line.separator"));
		} else {
			br = new BufferedReader(new InputStreamReader(new FileInputStream(
					input), encoding));
		}

		for (int i = 0; i < n; i++) {
			if (br.ready())
				bw.write(br.readLine() + System.getProperty("line.separator"));
			else {
				logger.warn("Value of number of lines were greater than " + n
						+ ".");
				br.close();
				bw.close();
				return;
			}
		}

		if (sentenceMarkers)
			bw.write("<s>");

		br.close();
		bw.close();
	}

	public static void chooseNRandomLines(File input, File output, int n,
			Charset encoding) throws FileNotFoundException, IOException {
		if (!input.exists() || !input.canRead())
			throw new IOException(
					"Cannot access file "
							+ input
							+ ". Check if the file exists or permissions have been set correctly.");

		BufferedReader br = new BufferedReader(new InputStreamReader(
				new FileInputStream(input), encoding));
		BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(output), encoding));

		boolean sentenceMarkers = false;

		if (br.readLine().equals("</s>")) {
			sentenceMarkers = true;
			bw.write("</s>" + System.getProperty("line.separator"));
		}
		br = new BufferedReader(new InputStreamReader(
				new FileInputStream(input), encoding));

		int numLines = countLines(input, encoding);

		if (n > numLines)
			throw new IllegalArgumentException(
					"Number of lines to choose is greater than number of lines in the file.");

		IntegerArray intArray = new IntegerArray(numLines);
		for (int i = 0; i < numLines; ++i)
			intArray.put(i, i);

		intArray.shuffle();

		String temp = null;
		if (sentenceMarkers) {
			for (int i = 0; i < numLines; i++) {
				temp = br.readLine();
				if (intArray.get(i) < n && !temp.equals("</s>")
						&& !temp.equals("<s>"))
					bw.write(temp + System.getProperty("line.separator"));

			}
		} else {
			for (int i = 0; i < numLines; i++) {
				if (intArray.get(i) < n)
					bw.write(br.readLine()
							+ System.getProperty("line.separator"));
				else
					br.readLine();
			}
		}

		if (sentenceMarkers)
			bw.write("<s>");
		br.close();
		bw.close();
	}

	public static void createTrainingTestSets(File corpus, File trainingSet,
			File testSet, boolean chooseRandomly, int testSetPercentage,
			Charset encoding) throws FileNotFoundException, IOException {
		if (!corpus.exists() || !corpus.canRead())
			throw new IOException(
					"Cannot access file "
							+ corpus
							+ ". Check if the file exists or permissions have been set correctly.");

		if (testSetPercentage < 0 || testSetPercentage > 100)
			throw new IllegalArgumentException(
					"Illegal test set percentage of " + testSetPercentage + ".");

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

		if (br.readLine().equals("</s>")) {
			sentenceMarkers = true;
			trainingWriter.write("</s>" + System.getProperty("line.separator"));
			testWriter.write("</s>" + System.getProperty("line.separator"));
		}

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
					if (!temp.equals("</s>") && !temp.equals("<s>")) {
						if (intArray.get(i) < trainingSetLines)
							trainingWriter.write(temp
									+ System.getProperty("line.separator"));
						else
							testWriter.write(temp
									+ System.getProperty("line.separator"));
					}
				}
			} else {
				for (int i = 0; i < numLines; i++) {
					if (intArray.get(i) < trainingSetLines)
						trainingWriter.write(br.readLine()
								+ System.getProperty("line.separator"));
					else
						testWriter.write(br.readLine()
								+ System.getProperty("line.separator"));
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
						+ System.getProperty("line.separator"));

			for (int i = trainingSetLines; i < numLines; ++i)
				testWriter.write(br.readLine()
						+ System.getProperty("line.separator"));
		}

		if (sentenceMarkers) {
			trainingWriter.write("<s>");
		}

		br.close();
		trainingWriter.close();
		testWriter.close();
	}

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
		String newline = System.getProperty("line.separator");

		BufferedReader reader = new BufferedReader(new InputStreamReader(
				new FileInputStream(inputFile), encoding));
		BufferedWriter writer = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(outputFile), encoding));

		writer.write("</s>" + newline);

		String temp = "";

		while ((temp = reader.readLine()) != null) {
			temp = temp.trim();
			if (temp.isEmpty())
				continue;

			writer.write("<s> " + temp + " </s>" + newline);
		}

		writer.write("<s>");

		reader.close();
		writer.close();

		/*
		 * String result = sb.toString();
		 * 
		 * return result.replaceAll("([^ ]{80})([^ ]{1,})", "\1");
		 */
	}

}
