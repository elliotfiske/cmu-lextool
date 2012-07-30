package edu.cmu.sphinx.sphingid.lm;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;

import java.lang.InterruptedException;

import java.nio.charset.Charset;

import java.util.ArrayList;

public class Main {

	public static void main(String[] args) {

		if (args.length == 0) {
			printUsage();
			return;
		}
		if (args[0].equals("combine")) {
			if (args.length >= 3) {
				ArrayList<File> files = new ArrayList<File>();

				for (int i = 1; i < args.length - 1; i++) {
					files.add(new File(args[i]));
				}
				try {
					FileUtils.concatenateFiles(files, args[args.length - 1]);
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				System.out.println("Usage: combine input1 input2 ... output");
				return;
			}
		} else if (args[0].equals("add-sentence-markers")) {
			if (args.length == 3)
				try {
					FileUtils.addSentenceMarkers(new File(args[1]), new File(
							args[2]), Charset.forName(System
							.getProperty("file.encoding")));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			else if (args.length == 4)
				try {
					FileUtils.addSentenceMarkers(new File(args[1]), new File(
							args[2]), Charset.forName(args[3]));
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			else {
				System.out
						.println("Usage: add-sentence-markers input output [encoding]\n\n"
								+ "Encoding property is optional but recommended. You can input an encoding by writing iso-8859-1 or utf-8");
				return;
			}
		} else if (args[0].equals("lm-data-selection")) {
			if (args.length >= 3) {
				File corpus = new File(args[1]);
				File inDomainCorpus = new File(args[2]);
				int testSetPercentage = 10;
				int n = 3;
				String smoothing = "improved-kneser-ney";
				Charset encoding = Charset.forName(System
						.getProperty("file.encoding"));
				int numSegments = 5;

				for (int i = 3; i < args.length; i += 2) {
					if (args[i].startsWith("--n")) {
						n = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--encoding")) {
						encoding = Charset.forName(args[i + 1]);
					} else if (args[i].startsWith("--smoothing")) {
						smoothing = args[i + 1];
					} else if (args[i].startsWith("--cutoffsegments")) {
						numSegments = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--testsetpercentage")) {
						testSetPercentage = Integer.parseInt(args[i + 1]);
					} else {
						System.out.println("Invalid parameter: " + args[i]);
						return;
					}
				}

				LMDataSelector dataSelector;
				try {
					dataSelector = new LMDataSelector(corpus, inDomainCorpus,
							testSetPercentage, n, smoothing, encoding);
					dataSelector.selectData(numSegments);
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				System.out
						.println("Usage: lm-data-selection gigantic-corpus in-domain-corpus [OPTIONS]\n\n"
								+ "Separates in domain corpus to training and test sets, constructs language models on in-domain corpus and a subset of main corpus, computes sentence-based perplexity differences of the entire corpus using two language models, separates it into n cutoff segments, creates language models for the segments, computes perplexities for the test set, compares it against random selection.\n\nOptions are:");
				System.out.printf("%-25s %s\n\n", "--n",
						"Number of tokens in an n-gram. (Default: 3)");
				System.out
						.printf("%-25s %s\n\n",
								"--encoding",
								"Specifies an encoding to use. Examples are iso-8859-1 or utf-8. Defaults to encoding of your system.");
				System.out
						.printf("%-25s %s\n\n",
								"--smoothing",
								"Specifies smoothing setting to use while building a language model with IRST LM. Possible options are witten-bell, kneser-ney, improved-kneser-ney. (Default: improved-kneser-ney)");
				System.out
						.printf("%-25s %s\n\n",
								"--cutoffsegments",
								"Specifies number of cutoff segments that the corpus will be separated to. (Default: 5)");
				System.out
						.printf("%-25s %s\n\n", "--testsetpercentage",
								"The amount of corpus that will be used as a test set. (Default: 10)");
				return;
			}
		} else if (args[0].equals("lm-data-selection-with-existing-models")) {
			if (args.length >= 5) {
				GiganticLanguageModel corpusModel = null;
				GiganticLanguageModel inDomainModel = null;

				try {
					corpusModel = new GiganticLanguageModel(new File(args[1]));
					inDomainModel = new GiganticLanguageModel(new File(args[2]));
				} catch (IOException e1) {
					// TODO Auto-generated catch block
					e1.printStackTrace();
				}
				File corpusFile = new File(args[3]);
				File testSet = new File(args[4]);
				int n = 3;
				String smoothing = "improved-kneser-ney";
				Charset encoding = Charset.forName(System
						.getProperty("file.encoding"));
				int numSegments = 5;

				for (int i = 3; i < args.length; i += 2) {
					if (args[i].startsWith("--n")) {
						n = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--encoding")) {
						encoding = Charset.forName(args[i + 1]);
					} else if (args[i].startsWith("--smoothing")) {
						smoothing = args[i + 1];
					} else if (args[i].startsWith("--cutoffsegments")) {
						numSegments = Integer.parseInt(args[i + 1]);
					} else {
						System.out.println("Invalid parameter: " + args[i]);
						return;
					}
				}
				LMDataSelector dataSelector;
				try {
					dataSelector = new LMDataSelector(corpusModel,
							inDomainModel, corpusFile, testSet, n, smoothing,
							encoding);
					dataSelector.selectData(numSegments);
				} catch (FileNotFoundException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				System.out
						.println("Usage: lm-data-selection-with-existing-models corpus-lm in-domain-lm corpus test-corpus [OPTIONS]\n\n"
								+ "Computes sentence-based perplexity differences of the entire corpus using two existing language models, separates it into n cutoff segments, creates language models for the segments, computes perplexities for the test set, compares it against random selection.\n\nThis method is not recommended if you have not constructed the language models with Sphingid. If you must use it, try to use models with constructed with exactly the same parameters.\n\nOptions are:");
				System.out.printf("%-25s %s\n\n", "--n",
						"Number of tokens in an n-gram. (Default: 3)");
				System.out
						.printf("%-25s %s\n\n",
								"--encoding",
								"Specifies an encoding to use. Examples are iso-8859-1 or utf-8. Defaults to encoding of your system.");
				System.out
						.printf("%-25s %s\n\n",
								"--smoothing",
								"Specifies smoothing setting to use while building a language model with IRST LM. Possible options are witten-bell, kneser-ney, improved-kneser-ney. (Default: improved-kneser-ney)");
				System.out
						.printf("%-25s %s\n\n",
								"--cutoffsegments",
								"Specifies number of cutoff segments that the corpus will be separated to. (Default: 5)");
				return;
			}
		} else if (args[0].equals("construct-lm")) {
			// TODO:Optional args
			if (args.length == 6) {
				File corpusFile = new File(args[1]);
				File lmFile = new File(args[2]);
				int n = Integer.parseInt(args[3]);
				String smoothing = args[4];
				Boolean backoff = Boolean.parseBoolean(args[5]);
				try {
					new LanguageModel(corpusFile, lmFile, n, smoothing, backoff);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				} catch (IOException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			} else {
				System.out
						.println("Usage: construct-lm corpus-file lm-output-file [n] [smoothing] [backoff]");
			}
		}
		// TODO : gigantic lm
		else {
			printUsage();
			return;
		}
	}

	public static void printUsage() {

		// TODO: update here
		System.out.println("Incorrect usage.");
		System.out
				.println("Correct arguments are listed below.\n"
						+ "Try each argument to get more detailed information.\n\n"
						+ "add-sentence-markers Adds sentence markers to each line in the file.\n\n"
						+ "combine Combines two files.\n\n"
						//+ "construct-lm Constructs an LM using IRST LM.\n\n"
						//+ "constuct-gigantic-lm Constructs an LM using build-lm.sh script of IRST LM. Useful for gigantic language models.\n\n"
						+ "lm-data-selection Selects sentences from a general corpus that are close to the in-domain data.\n\n"
						+ "lm-data-selection-with-existing-models Selects sentences from a general corpus that are close to in-domain data, using existing language models created for the corpus and in-domain data.");
	}
}
