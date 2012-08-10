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
			throw new IOException(
					Messages.getString("GiganticLanguageModel.CannotAccessFile") + lmFile); //$NON-NLS-1$
	}

	/**
	 * Builds a gigantic language model from corpusFile using build-lm.sh of
	 * IRST LM.
	 * 
	 * @param corpusFile
	 *            the corpus that will be used in training a language model
	 * @param lmFile
	 *            path to language model file that will be created * @param
	 *            dictionary dictionary file to use, can be null
	 * @param n
	 *            number of tokens in each n-gram
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param pruneSingletons
	 *            boolean value that determines if singleton n-grams will be
	 *            pruned or not
	 * @param sentenceMarkers
	 *            determines if sentence markers will be considered while
	 *            building the language model
	 * @param numSplits
	 *            number of corpus splits that build-lm.sh uses for using low
	 *            memory
	 * @throws InterruptedException
	 * @throws IOException
	 *             if corpusFile cannot be read or IRST LM tool cannot be
	 *             executed
	 */
	public GiganticLanguageModel(File corpusFile, File lmFile,
			Dictionary dictionary, int n, Smoothing smoothing,
			boolean pruneSingletons, boolean sentenceMarkers, int numSplits)
			throws InterruptedException, IOException {

		if (!(corpusFile.exists() && corpusFile.canRead())) {
			throw new FileNotFoundException(
					Messages.getString("CannotAccessCorpus")); //$NON-NLS-1$
		}
		
		File giganticLMScript = new File("irstlm/bin/build-lm.sh"); //$NON-NLS-1$

		if (!giganticLMScript.canRead() || !giganticLMScript.canExecute()) {
			throw new IOException(
					Messages.getString("GiganticLanguageModel.CannotAccessBuildLM")); //$NON-NLS-1$
		}

		String args = "-i " + corpusFile.getPath() + " -o " + lmFile.getPath() //$NON-NLS-1$ //$NON-NLS-2$
				+ " -k " + numSplits + " -n " + n; //$NON-NLS-1$ //$NON-NLS-2$

		switch (smoothing) {
		case WITTEN_BELL:
			args += " -s witten-bell"; //$NON-NLS-1$
			break;
		case SHIFT_BETA:
			args += " -s kneser-ney"; //$NON-NLS-1$
			break;
		case MODIFIED_SHIFT_BETA:
			args += " -s improved-kneser-ney"; //$NON-NLS-1$
			break;
		default:
			args += " -s improved-kneser-ney"; //$NON-NLS-1$
			break;
		}

		if (pruneSingletons)
			args += " -p"; //$NON-NLS-1$

		if (sentenceMarkers)
			args += " -b"; //$NON-NLS-1$

		if (dictionary != null) {
			if (dictionary.getDictFile().exists()
					&& dictionary.getDictFile().canRead())
				args += " -d " + dictionary.getDictFile().getPath(); //$NON-NLS-1$
			else {
				throw new IOException(
						Messages.getString("GiganticLanguageModel.CannotAccessDictionary")); //$NON-NLS-1$
			}
		}

		System.out.println(args);
		Process p = Runtime.getRuntime().exec(new String[] { "bash", "-c", //$NON-NLS-1$ //$NON-NLS-2$
				"./irstlm/bin/build-lm.sh " + args }, //$NON-NLS-1$
				new String[] { "IRSTLM=irstlm" }); //$NON-NLS-1$

		// p.waitFor();
		BufferedReader reader = new BufferedReader(new InputStreamReader(
				p.getInputStream()));
		int completion = 0;

		String temp = null;
		while ((temp = reader.readLine()) != null) {
			if (temp.startsWith("dict.")) { //$NON-NLS-1$
				logger.info(
						Messages.getString("GiganticLanguageModel.LMConstructionStatus"), completion * 50 //$NON-NLS-1$
								/ numSplits);
				completion++;
			}
		}
		logger.info(Messages
				.getString("GiganticLanguageModel.LMConstructionComplete")); //$NON-NLS-1$
		p.waitFor();

		this.lmFile = new File(lmFile + ".gz"); //$NON-NLS-1$
	}
}
