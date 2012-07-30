package edu.cmu.sphinx.sphingid.lm;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Language model class that represents language models built with tlm of IRST
 * LM.
 * 
 * @author Emre Çelikten
 */
public class LanguageModel extends AbstractLanguageModel {

	/**
	 * Builds a language model from corpusFile using IRST LM.
	 * 
	 * @param corpusFile
	 *            the corpus that will be used in training a language model
	 * @param lmFile
	 *            path to language model file that will be created
	 * @param n
	 *            number of tokens in each n-gram
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param backoff
	 *            a boolean value that sets backoff option of language model
	 *            training
	 * 
	 * @throws InterruptedException
	 * @throws IOException
	 *             if corpusFile cannot be read or IRST LM tool cannot be
	 *             executed
	 */
	public LanguageModel(File corpusFile, File lmFile, int n, String smoothing,
			boolean backoff) throws InterruptedException, IOException {
		// TODO: control smoothing strings, reject illegal ones with
		// IllegalArgumentException, possibly enums for safety

		// TODO: Add dict support etc, more tlm features

		if (!(corpusFile.exists() && corpusFile.canRead()))
			throw new FileNotFoundException(
					"Cannot access corpus file. Check if the file exists or permissions have been set correctly.");

		// TODO: Add write checks

		File tlm = new File("irstlm/bin/tlm");
		if (!tlm.canRead() || !tlm.canExecute()) {
			throw new IOException(
					"Cannot access tlm binary of IRSTLM. Check if the file exists or permissions have been set correctly.");
		}

		String bo;

		if (backoff)
			bo = "yes";
		else
			bo = "no";

		String args = "-tr=" + corpusFile.getPath() + " -n=" + n + " -lm="
				+ smoothing + " -bo=" + bo + " -o=" + lmFile.getPath();
		Process p = Runtime.getRuntime().exec(
				new String[] { "bash", "-c", "irstlm/bin/tlm " + args });

		p.waitFor();

		this.lmFile = lmFile;
	}

	/**
	 * Creates a LanguageModel instance using an existing model file.
	 * 
	 * @param lmFile
	 *            language model file to be used
	 * @throws IOException
	 *             if the file does not exist or cannot be read
	 */
	public LanguageModel(File lmFile) throws IOException {
		if (lmFile.exists() && lmFile.canRead())
			this.lmFile = lmFile;
		else
			throw new IOException("Cannot access file: " + lmFile);
	}
}
