package edu.cmu.sphinx.sphingid.lm;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Dictionary class for language models. Builds using IRST LM.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
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
					Messages.getString("CannotAccessCorpus")); //$NON-NLS-1$
		}

		if (pruneFrequency < 2 && pruneFrequency != -1)
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("Dictionary.IllegalPruneFrequency"), pruneFrequency)); //$NON-NLS-1$

		if (dictionaryLimit < 1 && dictionaryLimit != -1)
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("Dictionary.IllegalDictionaryLimit"), dictionaryLimit)); //$NON-NLS-1$

		File dict = new File("irstlm/bin/dict"); //$NON-NLS-1$
		if (!dict.canRead() || !dict.canExecute()) {
			throw new IOException(
					Messages.getString("Dictionary.CannotAccessDict")); //$NON-NLS-1$
		}

		String args = "-i=" + corpusFile.getPath() + " -o=" //$NON-NLS-1$ //$NON-NLS-2$
				+ dictFile.getPath();

		if (pruneFrequency != -1)
			args += " -pf=" + pruneFrequency; //$NON-NLS-1$

		if (dictionaryLimit != -1)
			args += " -pr=" + dictionaryLimit; //$NON-NLS-1$

		Process p = Runtime.getRuntime().exec(
				new String[] { "bash", "-c", "irstlm/bin/dict " + args }); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$

		p.waitFor();
		this.dictFile = dictFile;
	}

	/**
	 * Constructor for existing dictionaries.
	 * 
	 * @param dictFile
	 *            path to dictionary file that will be represented
	 */
	public Dictionary(File dictFile) {
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
