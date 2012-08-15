package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.InputStreamReader;
import java.io.IOException;
import java.util.ArrayList;

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

		ArrayList<String> command = new ArrayList<String>();
		command.add("irstlm/bin/build-lm.sh"); //$NON-NLS-1$
		command.add("-i"); //$NON-NLS-1$
		command.add(corpusFile.getPath());
		command.add("-o"); //$NON-NLS-1$
		command.add(lmFile.getPath());
		command.add("-k"); //$NON-NLS-1$
		command.add(String.valueOf(numSplits));
		command.add("-n"); //$NON-NLS-1$
		command.add(String.valueOf(n));
		command.add("-v"); //$NON-NLS-1$ 
		command.add("-s"); //$NON-NLS-1$
		switch (smoothing) {
		case WITTEN_BELL:
			command.add("witten-bell"); //$NON-NLS-1$
			break;
		case SHIFT_BETA:
			command.add("kneser-ney"); //$NON-NLS-1$
			break;
		case MODIFIED_SHIFT_BETA:
			command.add("improved-kneser-ney"); //$NON-NLS-1$
			break;
		default:
			command.add("improved-kneser-ney"); //$NON-NLS-1$
			break;
		}

		if (pruneSingletons)
			command.add("-p"); //$NON-NLS-1$

		if (sentenceMarkers)
			command.add("-b"); //$NON-NLS-1$

		if (dictionary != null) {
			if (dictionary.getDictFile().exists()
					&& dictionary.getDictFile().canRead()) {
				command.add("-d"); //$NON-NLS-1$
				command.add(dictionary.getDictFile().getPath());
			} else {
				throw new IOException(
						Messages.getString("GiganticLanguageModel.CannotAccessDictionary")); //$NON-NLS-1$
			}
		}

		command.trimToSize();
		ProcessBuilder pb = new ProcessBuilder(command);
		pb.environment().put("IRSTLM", "irstlm"); //$NON-NLS-1$ //$NON-NLS-2$
		Process p = pb.start();
		/*
		 * Process p = Runtime.getRuntime().exec(new String[] { "bash", "-c",
		 * "./irstlm/bin/build-lm.sh " + args }, new String[] { "IRSTLM=irstlm"
		 * }); *
		 */

		// p.waitFor();
		BufferedReader reader = new BufferedReader(new InputStreamReader(
				p.getInputStream()));
		BufferedReader errorReader = null;
		if (logger.isDebugEnabled()) {
			errorReader = new BufferedReader(
					new InputStreamReader(p.getInputStream()));
		}
		
		int completion = 0;

		String temp = null, error = null;
		while ((temp = reader.readLine()) != null) {
			if (logger.isDebugEnabled()) {
				logger.debug(temp);
				if ((error = errorReader.readLine()) != null) {
					logger.debug(error);
				}
			}
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
