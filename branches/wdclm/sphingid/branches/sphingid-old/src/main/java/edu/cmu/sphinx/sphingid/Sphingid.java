package edu.cmu.sphinx.sphingid;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;

import java.lang.reflect.InvocationTargetException;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.StringTokenizer;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.XMLConfiguration;

import org.apache.commons.lang.exception.ExceptionUtils;

import org.apache.hadoop.util.ToolRunner;

import org.apache.nutch.crawl.Crawl;
import org.apache.nutch.crawl.CrawlDb;
import org.apache.nutch.crawl.Generator;
import org.apache.nutch.crawl.Injector;

import org.apache.nutch.fetcher.Fetcher;

import org.apache.nutch.parse.ParseSegment;

import org.apache.nutch.segment.SegmentReader;

import org.apache.nutch.util.NutchConfiguration;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Sphingid {
	private static final Logger logger = LoggerFactory
			.getLogger(Sphingid.class);

	public static void main(String[] args) {
		logger.info("Sphingid 0.0.2");

		long initTime = System.currentTimeMillis();
		long startTime, endTime;

		/*
		 * Read configuration from config.xml
		 */
		XMLConfiguration configuration = null;

		try {
			configuration = new XMLConfiguration("config.xml");
		} catch (ConfigurationException e) {
			logger.error("Error at reading configuration file. Terminating...\n"
					+ ExceptionUtils.getStackTrace(e));
			System.exit(-1);
		}

		/*
		 * Check if configuration options are acceptable
		 */
		if (!validateConfiguration(configuration))
			System.exit(-1);

		logger.info("Configuration file validated.");

		/*
		 * Run crawl - standard, is replaced by incremental crawling
		 */
		/*
		 * if (configuration.getBoolean("crawl.enabled")) { String crawlArg =
		 * configuration.getString("paths.urls") + " -dir " +
		 * configuration.getString("paths.crawl") + " -threads " +
		 * configuration.getInt("crawl.fetchThreads") + " -depth " +
		 * configuration.getString("crawl.crawlDepth") + " -topN " +
		 * configuration.getString("crawl.crawlTopN");
		 * 
		 * startTime = System.currentTimeMillis();
		 * logger.info("Starting crawling at " + (startTime - initTime) +
		 * " ms into the execution.");
		 * 
		 * if (!runCrawl(crawlArg)) System.exit(-1);
		 * 
		 * endTime = System.currentTimeMillis();
		 * logger.info("Crawling successfully completed. Time elapsed in crawl: "
		 * + (endTime - startTime)/1000 + " seconds."); }
		 */

		// Run incremental crawl

		if (configuration.getBoolean("crawl.enabled")) {

			String crawlPath = configuration.getString("paths.crawl");
			int depth = configuration.getInt("crawl.crawlDepth");
			SegmentFilter segmentFilter = new SegmentFilter();

			if (!configuration.getBoolean("crawl.incrementalCrawl")) {
				/*
				 * Inject URLs into crawl database
				 */

				logger.info("Incremental crawl disabled. Injecting initial links from URLs file...");
				String urlsPath = configuration.getString("paths.urls");

				runInject(crawlPath + "/crawldb " + urlsPath);
			} else {
				logger.info("Incremental crawl enabled. Proceeding to generation step...");
			}

			startTime = System.currentTimeMillis();
			logger.info("Starting crawling at " + (startTime - initTime)
					+ " ms into the execution.");

			for (int i = 0; i < depth; i++) {

				startTime = System.currentTimeMillis();
				logger.info("Started to crawl at depth " + (i+1) + " out of " + depth + " at "
						+ (startTime - initTime) / 1000
						+ " seconds into the execution.");
				/*
				 * Generate new list to fetch from crawldb
				 */

				logger.info("Generating new fetch list..."); 
				runGenerate(crawlPath + "/crawldb " + crawlPath
						+ "/segments -topN "
						+ configuration.getInt("crawl.crawlTopN"));

				/*
				 * Get the newest segment directory
				 */

				File segmentsPath = new File(crawlPath + "/segments");

				File[] list = segmentsPath.listFiles(segmentFilter);
				Arrays.sort(list);
				File segmentDirectory = list[list.length - 1];
				logger.trace("Processing segment directory: "
						+ segmentDirectory.getPath());

				/*
				 * Fetch links
				 */

				logger.info("Fetching URLs..."); 
				runFetch(segmentDirectory.getPath() + " -threads "
						+ configuration.getInt("crawl.fetchThreads"));

				/*
				 * Parse segments
				 */

				logger.info("Parsing segments..."); 
				runParse(segmentDirectory.getPath());

				/*
				 * Update the database with new data
				 */

				logger.info("Updating crawl database..."); 
				runUpdateDB(crawlPath + "/crawldb "
						+ segmentDirectory.getPath());

				endTime = System.currentTimeMillis();
				logger.info("Crawling of depth " + i
						+ " successfully completed. Time elapsed in crawl: "
						+ (endTime - startTime) / 1000 + " seconds.");
			}

			endTime = System.currentTimeMillis();
			logger.info("Crawling successfully completed. Time elapsed in crawl: "
					+ (endTime - startTime) / 1000 + " seconds.");
		}

		/*
		 * Run dump
		 */

		if (configuration.getBoolean("dump.enabled")) {
			String dumpArg = "-dump "
					+ configuration.getString("paths.crawl") + "/segments/* "
					+ configuration.getString("paths.dump");

			if (!configuration.getBoolean("dump.dumpContent"))
				dumpArg += " -nocontent";
			if (!configuration.getBoolean("dump.dumpFetch"))
				dumpArg += " -nofetch";
			if (!configuration.getBoolean("dump.dumpGenerate"))
				dumpArg += " -nogenerate";
			if (!configuration.getBoolean("dump.dumpParse"))
				dumpArg += " -noparse";
			if (!configuration.getBoolean("dump.dumpParseData"))
				dumpArg += " -noparsedata";
			if (!configuration.getBoolean("dump.dumpParseText"))
				dumpArg += " -noparsetext";

			startTime = System.currentTimeMillis();
			logger.info("Starting dumping at " + (startTime - initTime) / 1000
					+ " seconds into the execution.");
			if (!runDump(dumpArg))
				System.exit(-1);
			endTime = System.currentTimeMillis();
			logger.info("Dumping successfully completed. Time elapsed in dump: "
					+ (endTime - startTime) / 1000 + " seconds.");
		}

		ArrayList<TextDocument> textDocumentList = null;

		/*
		 * Extract documents using boilerpipe
		 */

		if (configuration.getBoolean("extract.enabled")) {
			File nutchDump = new File(configuration.getString("paths.dump")
					+ "/dump");

			try {
				startTime = System.currentTimeMillis();
				logger.info("Starting extraction at " + (startTime - initTime)
						/ 1000 + " seconds into the execution.");

				textDocumentList = TextExtractor.extractNutchDump(nutchDump,
						configuration.getString("paths.extractedDocuments"),
						configuration.getString("extract.extractorClass"));

				endTime = System.currentTimeMillis();
				logger.info("Extraction successfully completed. Time elapsed in extraction: "
						+ (endTime - startTime) / 1000 + " seconds.");

			} catch (Exception e) {
				logger.error("An error was occurred in extraction step. Terminating...\n"
						+ ExceptionUtils.getStackTrace(e));
				System.exit(-1);
			}
			if (textDocumentList == null || textDocumentList.isEmpty()) {
				logger.error("Unknown error while extracting: No documents were extracted. Terminating...");
				System.exit(-1);
			}

		}

		if (configuration.getBoolean("addStartends.enabled")) {
			startTime = System.currentTimeMillis();
			logger.info("Starting adding start-ends at "
					+ (startTime - initTime) / 1000
					+ " seconds into the execution.");

			if (textDocumentList == null) {
				File hashFile = new File(
						configuration.getString("paths.extractedDocuments")
								+ "/hashtable");

				try {
					textDocumentList = FileUtils
							.readTextDocumentsFromHashtable(configuration
									.getString("paths.extractedDocuments"),
									hashFile, false);
				} catch (Exception e) {
					logger.error("An error was occurred in add start-ends step.\n"
							+ ExceptionUtils.getStackTrace(e));
					System.exit(-1);
				}
			}
			for (TextDocument document : textDocumentList) {
				try {
					IRSTLMWrapper.addStartEnd(document.getFile(), configuration
							.getString("paths.startendAddedDocuments"));
				} catch (IOException e) {
					logger.warn("Error at adding start-ends to file "
							+ document.getName() + ".\n"
							+ ExceptionUtils.getStackTrace(e));
				}
			}

			endTime = System.currentTimeMillis();
			logger.info("Adding start-ends successfully completed. Time elapsed in extraction: "
					+ (endTime - startTime) / 1000 + " seconds.");
		}

		if (configuration.getBoolean("kmeans.enabled")) {
			if (textDocumentList == null) {
				File hashFile = new File(
						configuration.getString("paths.hashtableFolder")
								+ "/hashtable");

				try {
					textDocumentList = FileUtils
							.readTextDocumentsFromHashtable(configuration
									.getString("paths.startendAddedDocuments"),
									hashFile, true);
				} catch (Exception e) {
					e.printStackTrace();
					System.exit(-1);
				}

			}

			startTime = System.currentTimeMillis();
			KMeans.IRSTLMCluster(textDocumentList,
					configuration.getInt("kmeans.numClusters"));
			endTime = System.currentTimeMillis();
			System.out.println("IRSTLM total execution time: "
					+ (endTime - startTime));
		}

		endTime = System.currentTimeMillis();
		logger.info("All operations in configuration have been completed. Total elapsed time: "
				+ (endTime - initTime) / 1000 + " seconds.");

	}

	public static boolean runCrawl(String args) {
		try {
			ToolRunner.run(NutchConfiguration.create(), new Crawl(),
					tokenize(args));
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	public static boolean runInject(String args) {
		try {
			ToolRunner.run(NutchConfiguration.create(), new Injector(),
					tokenize(args));
		} catch (Exception e) {
			logger.error("Error while injecting URLs into crawl database.\n"
					+ e.getStackTrace());
			return false;
		}
		return true;
	}

	public static boolean runGenerate(String args) {
		try {
			ToolRunner.run(NutchConfiguration.create(), new Generator(),
					tokenize(args));
		} catch (Exception e) {
			logger.error("Error while generating a fetch list from database.\n"
					+ e.getStackTrace());
			return false;
		}
		return true;
	}

	public static boolean runFetch(String args) {
		try {
			ToolRunner.run(NutchConfiguration.create(), new Fetcher(),
					tokenize(args));
		} catch (Exception e) {
			logger.error("Error while fetching URLs.\n" + e.getStackTrace());
			return false;
		}
		return true;
	}

	public static boolean runParse(String args) {
		try {
			ToolRunner.run(NutchConfiguration.create(), new ParseSegment(),
					tokenize(args));
		} catch (Exception e) {
			logger.error("Error while parsing segments.\n" + e.getStackTrace());
			return false;
		}
		return true;
	}

	public static boolean runUpdateDB(String args) {
		try {
			ToolRunner.run(NutchConfiguration.create(), new CrawlDb(),
					tokenize(args));
		} catch (Exception e) {
			logger.error("Error while parsing segments.\n" + e.getStackTrace());
			return false;
		}
		return true;
	}

	public static boolean runDump(String args) {
		try {
			SegmentReader.main(tokenize(args));
		} catch (Exception e) {
			e.printStackTrace();
			return false;
		}
		return true;
	}

	public static void runBenchmark(ArrayList<TextDocument> documents) {

		long startTime, endTime;

		File corpusFile = FileUtils.combineTextDocumentsToFile(documents,
				"combined/");

		File testFile = new File("startendadded/2114333595");

		File LMFile = new File("testLM");

		startTime = System.currentTimeMillis();
		IRSTLMWrapper.buildLM(corpusFile, LMFile, 3, "wb", false);
		endTime = System.currentTimeMillis();

		System.out.println("IRSTLM Training: Total execution time: "
				+ (endTime - startTime));

		startTime = System.currentTimeMillis();
		double irstlmPP = IRSTLMWrapper.computePerplexity(LMFile, testFile);
		endTime = System.currentTimeMillis();

		System.out.println("IRSTLM Perplexity: Total execution time: "
				+ (endTime - startTime));

		System.out.println("IRSTLM Perplexity:" + irstlmPP);

	}

	public static boolean runExtractDump(File nutchDumpFile, String outputPath,
			String extractorClass) {
		try {
			TextExtractor.extractNutchDump(nutchDumpFile, outputPath,
					extractorClass);
		} catch (IOException e) {
			e.printStackTrace();
			return false;
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
			return false;
		} catch (SecurityException e) {
			e.printStackTrace();
			return false;
		} catch (IllegalAccessException e) {
			e.printStackTrace();
			return false;
		} catch (InvocationTargetException e) {
			e.printStackTrace();
			return false;
		} catch (InstantiationException e) {
			e.printStackTrace();
			return false;
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
			return false;
		} catch (ClassNotFoundException e) {
			e.printStackTrace();
			return false;
		}

		return true;
	}

	/**
	 * Helper function to convert a string into an array of strings by
	 * separating them using whitespace.
	 * 
	 * @param str
	 *            string to be tokenized @return an array of strings that
	 *            contain a each word each
	 */
	public static String[] tokenize(String str) {
		StringTokenizer tok = new StringTokenizer(str);
		String tokens[] = new String[tok.countTokens()];
		int i = 0;
		while (tok.hasMoreTokens()) {
			tokens[i] = tok.nextToken();
			i++;
		}

		return tokens;

	}

	/**
	 * Checks if config.xml is valid.
	 * 
	 * TODO: Check file permissions and files
	 * 
	 * @param configuration
	 */
	private static boolean validateConfiguration(XMLConfiguration configuration) {

		// Paths

		if (configuration.getString("paths.temp").length() == 0) {
			logger.error("There seems to be a problem with your \"temp\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.urls").length() == 0) {
			logger.error("There seems to be a problem with your \"urls\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.crawl").length() == 0) {
			logger.error("There seems to be a problem with your \"crawl\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.dump").length() == 0) {
			logger.error("There seems to be a problem with your \"dump\" setting in paths for dumping. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.extractedDocuments").length() == 0) {
			logger.error("There seems to be a problem with your \"extractedDocuments\" setting in paths for extraction. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.startendAddedDocuments")
				.length() == 0) {
			logger.error("There seems to be a problem with your \"startendAddedDocuments\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.clusterLMs").length() == 0) {
			logger.error("There seems to be a problem with your \"clusterLMs\" setting in paths. Check your config.xml file.");
			return false;
		}

		// Crawl parameters
		if (configuration.getBoolean("crawl.enabled")) {
			if (configuration.getInt("crawl.fetchThreads") < 1) {
				logger.error("There seems to be a problem with your \"fetchThreads\" setting. Check your config.xml file.");
				return false;
			} else if (configuration.getInt("crawl.crawlDepth") < 1) {
				logger.error("There seems to be a problem with your \"crawlDepth\" setting. Check your config.xml file.");
				return false;
			} else if (configuration.getInt("crawl.crawlTopN") < 0) {
				logger.error("There seems to be a problem with your \"crawlTopN\" setting. Check your config.xml file.");
				return false;
			}
		}

		// Dump settings

		if (configuration.getBoolean("dump.enabled")) {

		}

		// Extractor settings

		if (configuration.getBoolean("extract.enabled")) {
			if (!(configuration.getString("extract.extractorClass")
					.equals("ArticleExtractor"))
					|| configuration.getString("extract.extractorClass")
							.equals("CanolaExtractor")
					|| configuration.getString("extract.extractorClass")
							.equals("DefaultExtractor")
					|| configuration.getString("extract.extractorClass")
							.equals("LargestContentExtractor")
					|| configuration.getString("extract.extractorClass")
							.equals("KeepEverythingExtractor")) {
				logger.error("You have specified an invalid extractor type "
						+ configuration.getString("extract.extractorClass")
						+ " for extractorClass. Possible candidates are ArticleExtractor, CanolaExtractor, DefaultExtractor, LargestContentExtractor and KeepEverythingExtractor.");
				return false;

			}
		}

		if (configuration.getBoolean("addStartends.enabled")) {
		}

		// KMeans settings

		if (configuration.getBoolean("kmeans.enabled")) {
			if (configuration.getInt("kmeans.numClusters") < 1) {
				logger.error("There seems to be a problem with your \"numClusters\" setting. Check your config.xml file.");
				return false;
			}
		}

		return true;

	}

	private static class SegmentFilter implements FilenameFilter {
		public boolean accept(File dir, String name) {
			if (name.matches("2.*"))
				return true;
			else
				return false;
		}
	}
}
