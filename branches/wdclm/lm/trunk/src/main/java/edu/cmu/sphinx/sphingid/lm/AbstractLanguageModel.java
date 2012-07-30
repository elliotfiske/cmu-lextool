package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.io.IOException;

import java.lang.InterruptedException;

import java.util.ArrayList;
import java.util.StringTokenizer;

/**
 * Abstract parent class for language models. Contains common perplexity
 * calculation methods.
 * 
 * @author Emre Çelikten
 */
public abstract class AbstractLanguageModel {
	protected File lmFile;

	/**
	 * Computes the perplexity of a test set against this language model.
	 * 
	 * @param testFile
	 *            the test set of which perplexity will be calculated
	 * @return the perplexity
	 * 
	 * @throws IOException
	 *             if testFile does not exists or cannot be read, if IRST LM
	 *             tools cannot be accessed
	 * @throws InterruptedException
	 */
	public final float computePerplexity(File testFile) throws IOException,
			InterruptedException {
		if (!testFile.exists() || !testFile.canRead())
			throw new IOException(
					"Cannot access file "
							+ testFile
							+ ". Check if the file exists or permissions have been set correctly.");

		File compile = new File("irstlm/bin/compile-lm");
		if (!compile.canRead() || !compile.canExecute())
			throw new IOException(
					"Cannot access compile-lm binary of IRSTLM. Check if the file exists or permissions have been set correctly.");

		Process p = Runtime.getRuntime().exec(
				new String[] {
						"bash",
						"-c",
						"irstlm/bin/compile-lm " + lmFile.getPath()
								+ " --eval=" + testFile.getPath() });
		p.waitFor();

		BufferedReader br = new BufferedReader(new InputStreamReader(
				p.getInputStream()));

		String result = null;

		String temp;
		while ((temp = br.readLine()) != null) {
			if (temp.startsWith("%%")) {
				result = temp;
				break;
			}

		}
		br.close();
		/*
		 * if (debug) { BufferedReader ebr = new BufferedReader(new
		 * InputStreamReader( p.getErrorStream()));
		 * 
		 * try { String temp; while ((temp = ebr.readLine()) != null) {
		 * System.out.println(temp); }
		 * 
		 * ebr.close(); } catch (IOException e) {
		 * ExceptionUtils.getStackTrace(e); System.exit(-1); } }
		 */

		StringTokenizer tok = new StringTokenizer(result);
		float perplexity = 0;
		while (tok.hasMoreTokens()) {
			String token = tok.nextToken();
			if (token.startsWith("PP=")) {
				perplexity = Float
						.parseFloat(token.substring(3, token.length()));
				break;
			}
		}
		return perplexity;
	}

	/**
	 * Computes the perplexity of a test set per sentence against this language
	 * model.
	 * 
	 * @param testFile
	 *            the test set of which perplexity will be calculated
	 * @return the perplexity values of each sentence
	 * 
	 * @throws IOException
	 *             if testFile does not exists or cannot be read, if IRST LM
	 *             tools cannot be accessed
	 * @throws InterruptedException
	 */
	public final ArrayList<SentencePerplexity> computeSentencePerplexities(
			File testFile) throws IOException, InterruptedException {
		// TODO: Move all of IRST LM checks to start of program
		if (!testFile.exists() || !testFile.canRead())
			throw new IOException(
					"Cannot access file "
							+ testFile
							+ ". Check if the file exists or permissions have been set correctly.");

		File compile = new File("irstlm/bin/compile-lm");
		if (!compile.canRead() || !compile.canExecute())
			throw new IOException(
					"Cannot access compile-lm binary of IRSTLM. Check if the file exists or permissions have been set correctly.");

		Process p = Runtime.getRuntime().exec(
				new String[] {
						"bash",
						"-c",
						"irstlm/bin/compile-lm " + lmFile.getPath()
								+ " --eval " + testFile.getPath()
								+ " --sentence yes" });

		BufferedReader br = new BufferedReader(new InputStreamReader(
				p.getInputStream()));
		// p.waitFor();

		ArrayList<SentencePerplexity> results = new ArrayList<SentencePerplexity>(
				50000000);

		int sentenceNum = 0;
		String temp;
		while ((temp = br.readLine()) != null) {
			if (temp.startsWith("%%")) {
				StringTokenizer tok = new StringTokenizer(temp);
				float perplexity = 0;
				while (tok.hasMoreTokens()) {
					String token = tok.nextToken();
					if (token.startsWith("sent_PP=")) {
						perplexity = Float.parseFloat(token.substring(8,
								token.length()));
						results.add(new SentencePerplexity(sentenceNum,
								perplexity));
						sentenceNum++;
						break;
					}
				}

			}

		}
		br.close();

		results.trimToSize();

		return results;
	}

	/**
	 * Gets the lmFile for this instance.
	 * 
	 * @return The lmFile.
	 */
	public File getLmFile() {
		return this.lmFile;
	}
}
