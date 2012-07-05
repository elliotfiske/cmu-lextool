package edu.cmu.sphinx.sphingid;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.StringReader;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;

import com.ibm.icu.util.StringTokenizer;

import de.l3s.boilerpipe.extractors.ExtractorBase;

/**
 * Extractor utility class for extracting documents from a Nutch dump.
 * 
 * @author Emre Çelikten
 */
public class TextExtractor {

	/**
	 * Extracts a Nutch dump file nutchDump. Each web page is saved as a text
	 * document under outputFolder. HTML extraction is done using Boilerpipe.
	 * 
	 * @param nutchDump
	 *            Designates dump file obtained from Nutch.
	 * @param outputFolder
	 *            Designates output folder into which extracted text documents
	 *            will be put.
	 * @param extractorClass
	 *            Provides which ExtractorClass of boilerpipe will be used.
	 *            (e.g. ArticleExtractor)
	 * @return an ArrayList object containing TextDocument objects for each
	 *         document in the dump.
	 * 
	 * @throws IOException
	 *             if there is a problem with disk access or reading files
	 * @throws IllegalArgumentException
	 * @throws IllegalAccessException
	 * @throws InvocationTargetException
	 * @throws InstantiationException
	 * @throws SecurityException
	 * @throws NoSuchMethodException
	 * @throws ClassNotFoundException
	 */
	public static ArrayList<TextDocument> extractNutchDump(File nutchDump,
			String outputFolder, String extractorClass) throws IOException,
			IllegalArgumentException, IllegalAccessException,
			InvocationTargetException, InstantiationException,
			SecurityException, NoSuchMethodException, ClassNotFoundException {

		ArrayList<TextDocument> list = new ArrayList<TextDocument>();

		/*
		 * Getting the extractor and its methods for extraction
		 */
		Object extractor;
		@SuppressWarnings("unchecked")
		Class<? extends ExtractorBase> extClass = (Class<? extends ExtractorBase>) Class
				.forName("de.l3s.boilerpipe.extractors." + extractorClass);
		extractor = extClass.getMethod("getInstance", null).invoke(null, null);

		Method textExtractionMethod;
		textExtractionMethod = extClass.getMethod("getText", String.class);

		/*
		 * Reading Nutch dump
		 */
		BufferedReader dumpReader = new BufferedReader(
				new FileReader(nutchDump));

		new File(outputFolder).mkdirs();

		/*
		 * Prepare for writing to hash table
		 */
		BufferedWriter hashWriter = new BufferedWriter(new FileWriter(
				outputFolder + "/hashtable"));

		String temp = "";
		StringBuilder htmlBuilder;
		StringTokenizer tokenizer;

		while (!temp.equals("Recno:: 0"))
			temp = dumpReader.readLine();

		long i = 0;

		while (dumpReader.ready()) {
			htmlBuilder = new StringBuilder();
			temp = dumpReader.readLine();
			i++;

			tokenizer = new StringTokenizer(temp);
			tokenizer.nextToken();
			String url = tokenizer.nextToken(); // URL::

			while (!temp.equals("Content:"))
				temp = dumpReader.readLine();

			temp = dumpReader.readLine();

			while (!temp.startsWith("Recno::") && dumpReader.ready()) {
				htmlBuilder.append(temp);
				temp = dumpReader.readLine();
			}

			String text, topic;
			String html = htmlBuilder.toString();

			

			/*
			 * Extract topic and text from the document using boilerpipe
			 */
			text = (String) textExtractionMethod.invoke(extractor, html);
			if (text.isEmpty()) {
				System.out.println("Empty record found at " + i
						+ ". Skipping...");
				continue;
			}
			BufferedReader topicReader = new BufferedReader(new StringReader(
					text));
			topic = topicReader.readLine();
			topicReader.close();

			/*
			 * Write to file
			 */
			File output = new File(outputFolder + "/"
					+ String.format("%012d", i));

			BufferedWriter bw = new BufferedWriter(new FileWriter(output));
			bw.write(text);
			bw.close();
			TextDocument newDoc = new TextDocument(output, topic, url);
			list.add(newDoc);

			/*
			 * Add the entry to hash table file in the format of "file url"
			 */
			hashWriter.write(output.getName() + " " + url + "\n");
		}

		dumpReader.close();
		hashWriter.close();

		return list;
	}

}
