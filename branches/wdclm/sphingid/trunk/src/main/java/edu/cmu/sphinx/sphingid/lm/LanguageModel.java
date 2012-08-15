package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * Language model class that represents language models built with tlm of IRST
 * LM.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 */
public class LanguageModel extends AbstractLanguageModel {
	private static final Logger logger = LoggerFactory
			.getLogger(LanguageModel.class);

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
			throw new IOException(
					Messages.getString("LanguageModel.13") + lmFile); //$NON-NLS-1$
	}

	/**
	 * Builds a language model from corpusFile using IRST LM.
	 * 
	 * @param corpusFile
	 *            the corpus that will be used in training a language model
	 * @param lmFile
	 *            path to language model file that will be created
	 * @param dictionary
	 *            dictionary file to use, can be null
	 * @param n
	 *            number of tokens in each n-gram
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param backoff
	 *            a boolean value that sets backoff option of language model
	 *            training
	 * @throws InterruptedException
	 * @throws IOException
	 *             if corpusFile cannot be read or IRST LM tool cannot be
	 *             executed
	 */
	public LanguageModel(File corpusFile, File lmFile, Dictionary dictionary,
			int n, Smoothing smoothing, boolean backoff)
			throws InterruptedException, IOException {
		if (!(corpusFile.exists() && corpusFile.canRead()))
			throw new FileNotFoundException(
					Messages.getString("LanguageModel.0")); //$NON-NLS-1$

		File tlm = new File("irstlm/bin/tlm"); //$NON-NLS-1$
		if (!tlm.canRead() || !tlm.canExecute()) {
			throw new IOException(Messages.getString("LanguageModel.2")); //$NON-NLS-1$
		}

		ArrayList<String> command = new ArrayList<String>();
		command.add("irstlm/bin/tlm"); //$NON-NLS-1$

		String bo;
		if (backoff)
			bo = "yes"; //$NON-NLS-1$
		else
			bo = "no"; //$NON-NLS-1$

		command.add("-tr=" + corpusFile.getPath()); //$NON-NLS-1$
		command.add("-n=" + n); //$NON-NLS-1$
		command.add("-bo=" + bo); //$NON-NLS-1$
		command.add("-o=" + lmFile.getPath()); //$NON-NLS-1$ 

		switch (smoothing) {
		case WITTEN_BELL:
			command.add("-lm=wb"); //$NON-NLS-1$
			break;
		case SHIFT_BETA:
			command.add("-lm=sb"); //$NON-NLS-1$
			break;
		case MODIFIED_SHIFT_BETA:
			command.add("-lm=msb"); //$NON-NLS-1$
			break;
		default:
			command.add("-lm=msb"); //$NON-NLS-1$
			break;
		}

		if (dictionary != null) {
			if (dictionary.getDictFile().exists()
					&& dictionary.getDictFile().canRead())
				command.add("-d=" + dictionary.getDictFile().getPath()); //$NON-NLS-1$
			else {
				throw new IOException(
						Messages.getString("GiganticLanguageModel.CannotAccessDictionary")); //$NON-NLS-1$
			}
		}
		ProcessBuilder pb = new ProcessBuilder(command);
		Process p = pb.start();

		BufferedReader br = new BufferedReader(new InputStreamReader(
				p.getErrorStream()));

		String temp;
		while ((temp = br.readLine()) != null) {
			logger.debug(temp);
		}

		p.waitFor();

		this.lmFile = lmFile;
	}
}
