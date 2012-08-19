/**
 * 
 */
package edu.cmu.sphinx.sphingid;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.nio.charset.Charset;
import java.util.ArrayList;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.XMLConfiguration;
import org.apache.commons.lang.exception.ExceptionUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import edu.cmu.sphinx.sphingid.commons.FileUtils;
import edu.cmu.sphinx.sphingid.crawler.Crawler;
import edu.cmu.sphinx.sphingid.dataselection.PerplexityBasedDataSelector;
import edu.cmu.sphinx.sphingid.lm.AbstractLanguageModel.Smoothing;
import edu.cmu.sphinx.sphingid.lm.GiganticLanguageModel;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class Sphingid {
	private static final Logger logger = LoggerFactory
			.getLogger(Sphingid.class);

	@SuppressWarnings("unused")
	public static void main(String[] args) {
		if (args.length == 0) {
			printUsage();
			return;
		}
		if (args[0].equals("combine")) { //$NON-NLS-1$
			if (args.length >= 3) {
				ArrayList<File> files = new ArrayList<File>();

				for (int i = 1; i < args.length - 1; i++) {
					files.add(new File(args[i]));
				}
				try {
					FileUtils.concatenateFiles(files, args[args.length - 1]);
				} catch (IOException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
			} else {
				System.out.println(Messages.getString("Sphingid.CombineUsage")); //$NON-NLS-1$
				return;
			}
		} else if (args[0].equals("add-sentence-markers")) { //$NON-NLS-1$
			if (args.length == 3)
				try {
					FileUtils.addSentenceMarkers(new File(args[1]), new File(
							args[2]), Charset.forName(System
							.getProperty("file.encoding"))); //$NON-NLS-1$
				} catch (IOException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
			else if (args.length == 4)
				try {
					FileUtils.addSentenceMarkers(new File(args[1]), new File(
							args[2]), Charset.forName(args[3]));
				} catch (IOException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
			else {
				System.out.println(Messages
						.getString("Sphingid.AddSentenceMarkersUsage")); //$NON-NLS-1$
				return;
			}
		} else if (args[0].equals("lm-data-selection")) { //$NON-NLS-1$
			if (args.length >= 3) {
				File corpus = new File(args[1]);
				File inDomainCorpus = new File(args[2]);
				int testSetPercentage = 10;
				int n = 3;
				Smoothing smoothing = Smoothing.MODIFIED_SHIFT_BETA;
				Charset encoding = Charset.forName(System
						.getProperty("file.encoding")); //$NON-NLS-1$
				int numSegments = 5;
				boolean useBuildLmScript = true;

				for (int i = 3; i < args.length; i += 2) {
					if (args[i].startsWith("--n")) { //$NON-NLS-1$
						n = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--encoding")) { //$NON-NLS-1$
						encoding = Charset.forName(args[i + 1]);
					} else if (args[i].startsWith("--smoothing")) { //$NON-NLS-1$
						String smoothingString = args[i + 1];
						if (smoothingString.equals("wb")) { //$NON-NLS-1$
							smoothing = Smoothing.WITTEN_BELL;
						} else if (smoothingString.equals("sb")) { //$NON-NLS-1$
							smoothing = Smoothing.SHIFT_BETA;
						} else if (smoothingString.equals("msb")) { //$NON-NLS-1$
							smoothing = Smoothing.MODIFIED_SHIFT_BETA;
						}
					} else if (args[i].startsWith("--cutoffsegments")) { //$NON-NLS-1$
						numSegments = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--testsetpercentage")) { //$NON-NLS-1$
						testSetPercentage = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--usetlm")) { //$NON-NLS-1$
						useBuildLmScript = false;
						i--;
					} else {
						System.out
								.println(Messages
										.getString("Sphingid.InvalidParameter") + args[i]); //$NON-NLS-1$
						return;
					}
				}

				PerplexityBasedDataSelector dataSelector;
				try {
					dataSelector = new PerplexityBasedDataSelector(corpus, inDomainCorpus,
							testSetPercentage, n, smoothing, numSegments, encoding, useBuildLmScript);
					dataSelector.selectData();
				} catch (FileNotFoundException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				} catch (IOException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				} catch (InterruptedException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
			} else {
				System.out.println(Messages
						.getString("Sphingid.LMDataSelectionUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", "--n", //$NON-NLS-1$ //$NON-NLS-2$
						Messages.getString("Sphingid.NUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--encoding", //$NON-NLS-1$
						Messages.getString("Sphingid.EncodingUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--smoothing", //$NON-NLS-1$
						Messages.getString("Sphingid.SmoothingUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--cutoffsegments", //$NON-NLS-1$
						Messages.getString("Sphingid.CutoffSegmentsUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", "--testsetpercentage", //$NON-NLS-1$ //$NON-NLS-2$
						Messages.getString("Sphingid.TestSetPercentageUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--usetlm", //$NON-NLS-1$
						Messages.getString("Sphingid.UseTlmUsage")); //$NON-NLS-1$
				return;
			}
		} else if (args[0].equals("lm-data-selection-wem")) { //$NON-NLS-1$
			if (args.length >= 5) {
				GiganticLanguageModel corpusModel = null;
				GiganticLanguageModel inDomainModel = null;

				try {
					corpusModel = new GiganticLanguageModel(new File(args[1]));
					inDomainModel = new GiganticLanguageModel(new File(args[2]));
				} catch (IOException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
				File corpusFile = new File(args[3]);
				File testSet = new File(args[4]);
				int n = 3;
				Smoothing smoothing = Smoothing.MODIFIED_SHIFT_BETA;
				Charset encoding = Charset.forName(System
						.getProperty("file.encoding")); //$NON-NLS-1$
				int numSegments = 5;
				boolean useBuildLmScript = true;

				for (int i = 3; i < args.length; i += 2) {
					if (args[i].startsWith("--n")) { //$NON-NLS-1$
						n = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--encoding")) { //$NON-NLS-1$
						encoding = Charset.forName(args[i + 1]);
					} else if (args[i].startsWith("--smoothing")) { //$NON-NLS-1$
						String smoothingString = args[i + 1];
						if (smoothingString.equals("wb")) { //$NON-NLS-1$
							smoothing = Smoothing.WITTEN_BELL;
						} else if (smoothingString.equals("sb")) { //$NON-NLS-1$
							smoothing = Smoothing.SHIFT_BETA;
						} else if (smoothingString.equals("msb")) { //$NON-NLS-1$
							smoothing = Smoothing.MODIFIED_SHIFT_BETA;
						}
					} else if (args[i].startsWith("--cutoffsegments")) { //$NON-NLS-1$
						numSegments = Integer.parseInt(args[i + 1]);
					} else if (args[i].startsWith("--usetlm")) { //$NON-NLS-1$
						useBuildLmScript = false;
						i--;
					} else {
						System.out
								.println(Messages
										.getString("Sphingid.InvalidParameter") + args[i]); //$NON-NLS-1$
						return;
					}
				}
				PerplexityBasedDataSelector dataSelector;
				try {
					dataSelector = new PerplexityBasedDataSelector(corpusModel,
							inDomainModel, corpusFile, testSet, n, smoothing,
							numSegments, encoding, useBuildLmScript);
					dataSelector.selectData();
				} catch (FileNotFoundException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				} catch (IOException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				} catch (InterruptedException e) {
					System.out.println(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
			} else {
				System.out
						.println(Messages
								.getString("Sphingid.LMDataSelectionWithExistingModelsUsage") //$NON-NLS-1$
								+ Messages
										.getString("Sphingid.LMDataSelectionWithExistingModelsDescription")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", "--n", //$NON-NLS-1$ //$NON-NLS-2$
						Messages.getString("Sphingid.NUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--encoding", //$NON-NLS-1$
						Messages.getString("Sphingid.EncodingUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--smoothing", //$NON-NLS-1$
						Messages.getString("Sphingid.SmoothingUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--cutoffsegments", //$NON-NLS-1$
						Messages.getString("Sphingid.CutoffSegmentsUsage")); //$NON-NLS-1$
				System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
						"--usetlm", //$NON-NLS-1$
						Messages.getString("Sphingid.UseTlmUsage")); //$NON-NLS-1$
				return;
			}
		} else if (args[0].equals("crawl")) { //$NON-NLS-1$
			if (args.length > 1) {
				String confPath = args[1];

				XMLConfiguration crawlerConfiguration = null;

				crawlerConfiguration = new XMLConfiguration();
				crawlerConfiguration.setFileName(confPath);
				crawlerConfiguration.setSchemaValidation(true);

				try {
					crawlerConfiguration.load();
				} catch (ConfigurationException e) {
					logger.error(Messages
							.getString("Sphingid.ConfigurationLoadError")); //$NON-NLS-1$
					logger.error(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}

				boolean incremental = false;
				if (args.length > 2) {
					if (args[2].startsWith("--incremental")) { //$NON-NLS-1$
						incremental = true;
					} else {
						System.out.println(Messages
								.getString("Sphingid.CrawlUsage")); //$NON-NLS-1$
					}
				}

				new Crawler(crawlerConfiguration, incremental);
			} else {
				System.out.println(Messages.getString("Sphingid.CrawlUsage")); //$NON-NLS-1$
				System.out.println(Messages
						.getString("Sphingid.CrawlDescription")); //$NON-NLS-1$
			}
		} else {
			printUsage();
			return;
		}
	}

	public static void printUsage() {
		// TODO: update here
		System.out.println(Messages.getString("Sphingid.IncorrectUsage"));//$NON-NLS-1$
		System.out.println(Messages
				.getString("Sphingid.CorrectArgumentsListedBelow")); //$NON-NLS-1$
		System.out
				.printf("%-25s %s\n\n", "add-sentence-markers", //$NON-NLS-1$ //$NON-NLS-2$
						Messages.getString("Sphingid.AddSentenceMarkersShortDescription")); //$NON-NLS-1$
		System.out
				.printf("%-25s %s\n\n", "combine", Messages.getString("Sphingid.CombineShortDescription")); //$NON-NLS-1$ //$NON-NLS-2$ //$NON-NLS-3$

		System.out.printf("%-25s %s\n\n", "lm-data-selection", //$NON-NLS-1$ //$NON-NLS-2$
				Messages.getString("Sphingid.LMDataSelectionShortDescription")); //$NON-NLS-1$
		System.out
				.printf("%-25s %s\n\n", //$NON-NLS-1$
						"lm-data-selection-wem", Messages.getString("Sphingid.LMDataSelectionWithExistingModelsShortDescription")); //$NON-NLS-1$ //$NON-NLS-2$
		System.out.printf("%-25s %s\n\n", //$NON-NLS-1$
				"crawl", Messages.getString("Sphingid.CrawlShortDescription")); //$NON-NLS-1$ //$NON-NLS-2$

		// + "construct-lm Constructs an LM using IRST LM.\n\n"
		// +
		// "constuct-gigantic-lm Constructs an LM using build-lm.sh script of IRST LM. Useful for gigantic language models.\n\n"
	}
}
