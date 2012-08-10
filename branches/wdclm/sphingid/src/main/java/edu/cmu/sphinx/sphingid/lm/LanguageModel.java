package edu.cmu.sphinx.sphingid.lm;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

/**
 * Language model class that represents language models built with tlm of IRST
 * LM.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 */
public class LanguageModel extends AbstractLanguageModel {

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

		if (!lmFile.canWrite()) {
			throw new IOException(Messages.getString("CannotWriteToModelFile")); //$NON-NLS-1$
		}

		File tlm = new File("irstlm/bin/tlm"); //$NON-NLS-1$
		if (!tlm.canRead() || !tlm.canExecute()) {
			throw new IOException(Messages.getString("LanguageModel.2")); //$NON-NLS-1$
		}

		String bo;
		if (backoff)
			bo = "yes"; //$NON-NLS-1$
		else
			bo = "no"; //$NON-NLS-1$

		String args = "-tr=" + corpusFile.getPath() + " -n=" + n + //$NON-NLS-1$ //$NON-NLS-2$
				" -bo=" + bo + " -o=" + lmFile.getPath(); //$NON-NLS-1$ //$NON-NLS-2$

		switch (smoothing) {
		case WITTEN_BELL:
			args += " -lm=wb"; //$NON-NLS-1$
			break;
		case SHIFT_BETA:
			args += " -lm=sb"; //$NON-NLS-1$
			break;
		case MODIFIED_SHIFT_BETA:
			args += " -lm=msb"; //$NON-NLS-1$
			break;
		default:
			args += " -lm=msb"; //$NON-NLS-1$
			break;
		}

		if (dictionary != null) {
			if (dictionary.getDictFile().exists()
					&& dictionary.getDictFile().canRead())
				args += " -d=" + dictionary.getDictFile().getPath(); //$NON-NLS-1$
			else {
				throw new IOException(
						Messages.getString("GiganticLanguageModel.CannotAccessDictionary")); //$NON-NLS-1$
			}
		}

		Process p = Runtime.getRuntime().exec(
				new String[] { "bash", "-c", "irstlm/bin/tlm " + args }); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$

		p.waitFor();

		this.lmFile = lmFile;
	}
}
