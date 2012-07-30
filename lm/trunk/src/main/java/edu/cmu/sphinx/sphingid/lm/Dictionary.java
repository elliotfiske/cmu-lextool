package edu.cmu.sphinx.sphingid.lm;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Dictionary class for language models. Builds using IRST LM.
 * 
 * @author Emre Çelikten
 */
public class Dictionary {
	private File dictFile;

	/**
	 * Creates a Dictionary object by extracting a dictionary from corpusFile
	 * using IRST LM.
	 * 
	 * @param corpusFile
	 *            the corpus that contains words to be extracted
	 * @param dictFile
	 *            path to dictionary file that will be created
	 * @param pruneFrequency
	 *            the frequency than which words occur less will be pruned
	 * @param dictionaryLimit
	 *            number of words in total, only the most frequent will be kept
	 * 
	 * @throws IOException
	 *             if corpusFile cannot be read or IRST LM tool cannot be
	 *             executed
	 * @throws InterruptedException
	 */
	public Dictionary(File corpusFile, File dictFile, int pruneFrequency,
			int dictionaryLimit) throws IOException, InterruptedException {
		if (!(corpusFile.exists() && corpusFile.canRead())) {
			throw new FileNotFoundException(
					"Cannot access corpus file. Check if the file exists or permissions have been set correctly.");
		}

		if (pruneFrequency < 2 && pruneFrequency != -1)
			throw new IllegalArgumentException("Illegal prune frequency of "
					+ pruneFrequency + ".");

		if (dictionaryLimit < 1 && dictionaryLimit != -1)
			throw new IllegalArgumentException("Illegal dictionary limit of "
					+ dictionaryLimit + ".");

		File dict = new File("irstlm/bin/dict");
		if (!dict.canRead() || !dict.canExecute()) {
			throw new IOException(
					"Cannot access dict binary of IRSTLM. Check if the file exists or permissions have been set correctly. Terminating...");
		}

		String args = "-i=" + corpusFile.getPath() + " -o="
				+ dictFile.getPath();

		if (pruneFrequency != -1)
			args += " -pf=" + pruneFrequency;

		if (dictionaryLimit != -1)
			args += " -pr=" + dictionaryLimit;

		Process p = Runtime.getRuntime().exec(
				new String[] { "bash", "-c", "irstlm/bin/dict " + args });

		p.waitFor();
		this.dictFile = dictFile;
	}

	/**
	 * Gets the File object pointing to dictionary file in the system for this
	 * instance.
	 * 
	 * @return The dictFile.
	 */
	public File getDictFile() {
		return this.dictFile;
	}
}
