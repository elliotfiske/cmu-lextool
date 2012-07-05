package edu.cmu.sphinx.sphingid;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.StringTokenizer;

public class IRSTLMWrapper {

	public static boolean debug = false;

	public static File addStartEnd(File file, String outputFolder) {
		Process p = null;

		new File(outputFolder).mkdirs();
		StringBuilder result = new StringBuilder();

		try {
			p = Runtime
					.getRuntime()
					.exec(new String[] { "bash", "-c",
							"irstlm/add-start-end.sh < " + file.getPath() });

		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}

		BufferedReader br = new BufferedReader(new InputStreamReader(
				p.getInputStream()));

		try {
			String temp;
			while ((temp = br.readLine()) != null) {
				result.append(temp + "\n");
			}

			br.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}

		if (result.length() == 0) {
			System.out.println("Empty file: " + file.getPath());
			System.exit(-1);
		}

		File outputFile = new File(outputFolder + "/" + file.getName());

		try {
			BufferedWriter bw = new BufferedWriter(new FileWriter(outputFile));
			bw.write(result.toString());
			bw.close();
		} catch (IOException e1) {
			e1.printStackTrace();
			System.exit(-1);
		}

		return outputFile;
	}

	public static void buildLM(File trainingFile, File LMFile, int ngramCount,
			String smoothing, boolean backoff) {

		if (!trainingFile.exists()) {
			System.out.println("Training file does not exist!");
			System.exit(-1);
		}

		if (LMFile.getParentFile() != null)
			LMFile.getParentFile().mkdirs();

		Process p;
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
							"irstlm/tlm -tr=" + trainingFile.getPath()
									+ " -n=" + ngramCount + " -lm=" + smoothing
									+ " -bo=" + bo + " -obin="
									+ LMFile.getPath() });

		} catch (IOException e) {
			e.printStackTrace();
			return;
		}

		try {
			p.waitFor();
		} catch (InterruptedException e) {
			e.printStackTrace();
		}

		if (debug) {
			BufferedReader ebr = new BufferedReader(new InputStreamReader(
					p.getErrorStream()));

			try {
				String temp;
				while ((temp = ebr.readLine()) != null) {
					System.out.println(temp);
				}

				ebr.close();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(-1);
			}
		}
	}

	public static double computePerplexity(File LMFile, File testFile) {

		if (!LMFile.exists()) {
			System.out.println("LM file does not exist!");
			System.exit(-1);
		}

		if (!testFile.exists()) {
			System.out.println("Test file does not exist!");
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
			e.printStackTrace();
			System.exit(-1);
		}

		try {
			p.waitFor();
		} catch (InterruptedException e) {
			e.printStackTrace();
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
			e.printStackTrace();
			System.exit(-1);
		}

		if (debug) {
			BufferedReader ebr = new BufferedReader(new InputStreamReader(
					p.getErrorStream()));

			try {
				String temp;
				while ((temp = ebr.readLine()) != null) {
					System.out.println(temp);
				}

				ebr.close();
			} catch (IOException e) {
				e.printStackTrace();
				System.exit(-1);
			}
		}

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

		perplexity = pp - ppwp;
		// perplexity = pp;

		return perplexity;
	}
}
