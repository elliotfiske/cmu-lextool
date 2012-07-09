package edu.cmu.sphinx.sphingid;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.IOException;

import java.util.StringTokenizer;

import org.apache.commons.lang.exception.ExceptionUtils;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * IRST LM wrapper class. Provides methods for interfacing to IRST LM.
 * 
 * @author Emre Çelikten
 * 
 */
public class IRSTLMWrapper {
	private static final Logger logger = LoggerFactory
			.getLogger(IRSTLMWrapper.class);

	/**
	 * Uses IRST LM add-start-end.sh on a file.
	 * 
	 * @param file
	 *            file to be processed
	 * @param outputFolder
	 *            path to folder into where the result will be put
	 * @return the resulting file, for convenience
	 * @throws IOException
	 */
	public static File addStartEnd(File file, String outputFolder)
			throws IOException {
		Process p = null;

		new File(outputFolder).mkdirs();
		StringBuilder result = new StringBuilder();

		File script = new File("irstlm/add-start-end.sh");
		if (!script.canRead()) {
			logger.error("Cannot access IRSTLM start-end script. Check if the file exists or permissions have been set correctly. Terminating...");
			System.exit(-1);
		}

		try {
			p = Runtime.getRuntime().exec(
					new String[] { "bash", "-c",
							"irstlm/add-start-end.sh < " + file.getPath() });
		} catch (IOException e) {
			logger.error("Error at interfacing to add-start-end.sh. Terminating...\n"
					+ ExceptionUtils.getStackTrace(e));
			System.exit(-1);
		}

		BufferedReader br = new BufferedReader(new InputStreamReader(
				p.getInputStream()));

		String temp;
		while ((temp = br.readLine()) != null) {
			result.append(temp + "\n");
		}

		br.close();

		if (result.length() == 0) {
			logger.warn("Empty file: " + file.getPath());
			throw new IOException();
		}

		File outputFile = new File(outputFolder + "/" + file.getName());

		try {
			BufferedWriter bw = new BufferedWriter(new FileWriter(outputFile));
			bw.write(result.toString());
			bw.close();
		} catch (IOException e) {
			logger.error("Unable to write to output file. Terminating...\n" + ExceptionUtils.getStackTrace(e));
			System.exit(-1);
		}

		return outputFile;
	}

	public static void buildLM(File trainingFile, File LMFile, int ngramCount,
			String smoothing, boolean backoff) {

		if (!trainingFile.exists()) {
			logger.error("Training file does not exist!");
			System.exit(-1);
		}

		if (LMFile.getParentFile() != null)
			LMFile.getParentFile().mkdirs();

		File tlm = new File("irstlm/tlm");
		if (!tlm.canRead()) {
			logger.error("Cannot access tlm binary of IRSTLM. Check if the file exists or permissions have been set correctly. Terminating...");
			System.exit(-1);
		}

		Process p = null;
		String bo;

		if (backoff)
			bo = "yes";
		else
			bo = "no";

		try {
			p = Runtime.getRuntime().exec(
					new String[] {
							"bash",
							"-c",
							"irstlm/tlm -tr=" + trainingFile.getPath() + " -n="
									+ ngramCount + " -lm=" + smoothing
									+ " -bo=" + bo + " -obin="
									+ LMFile.getPath() });

		} catch (IOException e) {
			logger.error("Error at interfacing to tlm. Terminating...\n"
					+ ExceptionUtils.getStackTrace(e));
			System.exit(-1);
		}

		try {
			p.waitFor();
		} catch (InterruptedException e) {
			logger.error(ExceptionUtils.getStackTrace(e));
		}

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
	}

	public static double computePerplexity(File LMFile, File testFile) {

		if (!LMFile.exists()) {
			logger.error("LM file does not exist!");
			System.exit(-1);
		}

		if (!testFile.exists()) {
			logger.error("Test file does not exist!");
			System.exit(-1);
		}

		File compile = new File("irstlm/compile-lm");
		if (!compile.canRead()) {
			logger.error("Cannot access compile-lm binary of IRSTLM. Check if the file exists or permissions have been set correctly. Terminating...");
			System.exit(-1);
		}

		Process p = null;
		try {

			p = Runtime.getRuntime().exec(
					new String[] {
							"bash",
							"-c",
							"irstlm/compile-lm " + LMFile.getPath()
									+ " --eval=" + testFile.getPath() });
		} catch (IOException e) {
			logger.error("Error at interfacing to tlm. Terminating...\n"
					+ ExceptionUtils.getStackTrace(e));
			System.exit(-1);
		}

		try {
			p.waitFor();
		} catch (InterruptedException e) {
			logger.error(ExceptionUtils.getStackTrace(e));
		}

		BufferedReader br = new BufferedReader(new InputStreamReader(
				p.getInputStream()));

		String result = null;

		try {
			String temp;
			while ((temp = br.readLine()) != null) {
				if (temp.startsWith("%%")) {
					result = temp;
					break;
				}

			}
			br.close();
		} catch (IOException e) {
			logger.warn("Error while reading output from compile-lm. Terminating...\n" + ExceptionUtils.getStackTrace(e));
			System.exit(-1);
		}

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
		Double perplexity = null, pp = null, ppwp = null;
		while (tok.hasMoreTokens()) {
			String token = tok.nextToken();
			if (token.startsWith("PP=")) {
				pp = Double.parseDouble(token.substring(3, token.length()));
				token = tok.nextToken();
				ppwp = Double.parseDouble(token.substring(5, token.length()));
				break;
			}
		}

		// perplexity = pp - ppwp;
		perplexity = pp;

		return perplexity;
	}
}
