package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.io.IOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class GiganticLanguageModel extends AbstractLanguageModel {
	private static final Logger logger = LoggerFactory
			.getLogger(GiganticLanguageModel.class);

	/**
	 * Builds a gigantic language model from corpusFile using build-lm.sh of
	 * IRST LM.
	 * 
	 * @param corpusFile
	 *            the corpus that will be used in training a language model
	 * @param lmFile
	 *            path to language model file that will be created
	 * @param n
	 *            number of tokens in each n-gram
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param pruneSingletons
	 *            boolean value that determines if singleton n-grams will be
	 *            pruned or not
	 * @param dictFile
	 *            dictionary file to use, can be null
	 * @param numSplits
	 *            number of corpus splits that build-lm.sh uses for using low
	 *            memory
	 * @throws InterruptedException
	 * @throws IOException
	 *             if corpusFile cannot be read or IRST LM tool cannot be
	 *             executed
	 */
	public GiganticLanguageModel(File corpusFile, File lmFile, int n,
			String smoothing, boolean pruneSingletons, boolean sentenceMarkers,
			Dictionary dictionary, int numSplits) throws InterruptedException,
			IOException {
		// TODO: control smoothing strings, reject illegal ones with
		// IllegalArgumentException, possibly enums for safety
		if (!(corpusFile.exists() && corpusFile.canRead())) {
			throw new FileNotFoundException(
					"Cannot access corpus file. Check if the file exists or permissions have been set correctly.");
		}

		File giganticLMScript = new File("irstlm/bin/build-lm.sh");

		if (!giganticLMScript.canRead() || !giganticLMScript.canExecute()) {
			throw new IOException(
					"Cannot access build-lm.sh of IRST LM. Check if the file exists or permissions have been set correctly.");
		}

		String args = "-i " + corpusFile.getPath() + " -o " + lmFile.getPath()
				+ " -k " + numSplits + " -n " + n;
		if (smoothing != null)
			args += " -s " + smoothing;

		if (pruneSingletons)
			args += " -p";

		if (sentenceMarkers)
			args += " -b";

		if (dictionary != null)
			if (dictionary.getDictFile().exists()
					&& dictionary.getDictFile().canRead())
				args += " -d " + dictionary.getDictFile().getPath();
			else {
				throw new IOException(
						"Cannot access dictionary file. Check if the file exists or permissions have been set correctly.");
			}

		System.out.println(args);
		Process p = Runtime.getRuntime()
				.exec(new String[] { "bash", "-c",
						"./irstlm/bin/build-lm.sh " + args },
						new String[] { "IRSTLM=irstlm" });

		// p.waitFor();
		BufferedReader reader = new BufferedReader(new InputStreamReader(
				p.getInputStream()));
		int completion = 0;

		String temp = null;
		while ((temp = reader.readLine()) != null) {
			if (temp.startsWith("dict.")) {
				logger.info("LM construction is at " + completion * 50
						/ numSplits + "%");
				completion++;
			}
		}
		logger.info("LM construction is at 100%");
		p.waitFor();

		this.lmFile = new File(lmFile + ".gz");
	}

	/**
	 * Create a GiganticLanguageModel instance with an existing model file.
	 * 
	 * @param lmFile
	 *            language model file to be used
	 * @throws IOException
	 *             if the file does not exist or cannot be read
	 */
	public GiganticLanguageModel(File lmFile) throws IOException {
		if (lmFile.exists() && lmFile.canRead())
			this.lmFile = lmFile;
		else
			throw new IOException("Cannot access file: " + lmFile);
	}
}
