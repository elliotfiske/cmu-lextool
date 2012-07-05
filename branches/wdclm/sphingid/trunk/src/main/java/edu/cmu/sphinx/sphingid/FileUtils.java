package edu.cmu.sphinx.sphingid;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import java.util.ArrayList;
import java.util.StringTokenizer;

public class FileUtils {
	public static File combineTextDocumentsToFile(
			ArrayList<TextDocument> documents, String path) {
		String result = combineTextDocumentsToString(documents);
		String filename = "";

		for (TextDocument document : documents) {
			filename += document.getName();
		}

		new File(path).mkdirs();
		File resultFile = new File(path + filename.hashCode());

		try {
			BufferedWriter bw = new BufferedWriter(new FileWriter(resultFile));
			bw.write(result);
			bw.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}

		return resultFile;

	}

	public static String combineTextDocumentsToString(
			ArrayList<TextDocument> documents) {
		StringBuilder builder = new StringBuilder(10000);

		builder.append("</s>\n");

		for (TextDocument document : documents) {
			BufferedReader br = null;
			try {
				br = new BufferedReader(new FileReader(document.getFile()));
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
			String temp = "";

			try {
				while (!temp.equals("</s>") && br.ready())
					temp = br.readLine();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(-1);
			}

			try {
				while (br.ready()) {
					temp = br.readLine();
					builder.append(temp + "\n");

				}
				br.close();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(-1);
			}
			builder.delete(builder.lastIndexOf("<s>"), builder.length());

		}

		builder.append("<s>\n");

		return builder.toString();
	}

	public static String[] combineTextDocumentsToStrArray(
			ArrayList<TextDocument> documents) {
		ArrayList<String> sentenceList = new ArrayList<String>();

		sentenceList.add("</s>\n");

		for (TextDocument document : documents) {
			BufferedReader br = null;
			try {
				br = new BufferedReader(new FileReader(document.getFile()));
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			}
			String temp = "";

			try {
				while (!temp.equals("</s>") && br.ready())
					temp = br.readLine();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(-1);
			}

			try {
				while (br.ready()) {
					temp = br.readLine();
					sentenceList.add(temp + "\n");

				}
				br.close();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(-1);
			}
			sentenceList.remove(sentenceList.size() - 1);
		}

		sentenceList.add("<s>\n");
		String[] sentenceArray = new String[sentenceList.size()];
		sentenceList.toArray(sentenceArray);

		return sentenceArray;
	}

	public static String[] readTextDocumentAsStrArray(TextDocument document) {

		ArrayList<String> sentenceList = new ArrayList<String>();

		sentenceList.add("</s>\n");

		BufferedReader br = null;
		try {
			br = new BufferedReader(new FileReader(document.getFile()));
		} catch (FileNotFoundException e) {
			e.printStackTrace();
		}
		String temp = "";

		try {
			while (!temp.equals("</s>") && br.ready())
				temp = br.readLine();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}

		try {
			while (br.ready()) {
				temp = br.readLine();
				sentenceList.add(temp + "\n");

			}
			br.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}
		sentenceList.remove(sentenceList.size() - 1);

		sentenceList.add("<s>\n");
		String[] sentenceArray = new String[sentenceList.size()];
		sentenceList.toArray(sentenceArray);

		return sentenceArray;
	}

	public static File writeFileListAsText(ArrayList<TextDocument> documents,
			String path) {
		StringBuilder builder = new StringBuilder(10000);

		for (TextDocument document : documents) {
			builder.append(document.getFile().getPath() + "\n");
		}

		new File(path).mkdirs();
		File file = new File("list.txt");

		try {
			BufferedWriter bw = new BufferedWriter(new FileWriter(file));
			bw.write(builder.toString());
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}

		return file;
	}

	public static ArrayList<TextDocument> readTextDocumentsFromHashtable(
			String documentFolder, File hashFile,
			boolean isStartEndAddedDocuments) throws FileNotFoundException,
			IOException {
		if (!hashFile.canRead())
			throw new FileNotFoundException(
					"Cannot read hash table, check your paths settings.");

		BufferedReader hashFileReader = new BufferedReader(new FileReader(
				hashFile));
		ArrayList<TextDocument> list = new ArrayList<TextDocument>();
		StringTokenizer tok;
		String name, topic, url;

		while (hashFileReader.ready()) {
			tok = new StringTokenizer(hashFileReader.readLine());
			name = tok.nextToken();
			url = tok.nextToken();

			File document = new File(documentFolder + "/" + name);
			if (!document.canRead())
				throw new FileNotFoundException(
						"Cannot read file in hash table: "
								+ name
								+ ". Check if the file exists and permissions are set correctly.");

			BufferedReader documentReader = new BufferedReader(new FileReader(
					document));
			if (documentReader.ready()) {
				if (isStartEndAddedDocuments)
					topic = documentReader.readLine().replaceAll("</?s>", "");
				else
					topic = documentReader.readLine();
			} else
				throw new IOException("Cannot read file: " + name
						+ ". Unknown IO problem.");

			list.add(new TextDocument(document, topic, url));

		}
		return list;
	}

}
