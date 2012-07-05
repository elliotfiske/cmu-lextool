package edu.cmu.sphinx.sphingid;

import java.io.File;
import java.io.IOException;
import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.StringTokenizer;

import org.apache.commons.configuration.ConfigurationException;
import org.apache.commons.configuration.XMLConfiguration;
import org.apache.hadoop.util.ToolRunner;
import org.apache.nutch.crawl.Crawl;
import org.apache.nutch.segment.SegmentReader;
import org.apache.nutch.util.NutchConfiguration;

public class Sphingid {

	public static void main(String[] args) {
		System.out.println("Sphingid 0.0.1.");

		// Read configuration from config.xml
		XMLConfiguration configuration = null;
		try {
			configuration = new XMLConfiguration("config.xml");
		} catch (ConfigurationException e) {
			e.printStackTrace();
		}

		// Check if configuration options are acceptable
		if (!validateConfiguration(configuration))
			System.exit(-1);

		// Run crawl
		if (configuration.getBoolean("crawl.enabled")) {
			String crawlArg = configuration.getString("paths.urls") + " -dir "
					+ configuration.getString("paths.crawl") + " -threads "
					+ configuration.getInt("crawl.crawlThreads") + " -depth "
					+ configuration.getString("crawl.crawlDepth") + " -topN "
					+ configuration.getString("crawl.crawlTopN");

			if (!runCrawl(crawlArg))
				System.exit(-1);
		}

		// Run dump
		if (configuration.getBoolean("dump.enabled")) {
			String dumpArg = "-dump "
					+ configuration.getString("paths.segments") + "/* "
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

			if (!runDump(dumpArg))
				System.exit(-1);

		}

		ArrayList<TextDocument> textDocumentList = null;

		if (configuration.getBoolean("extract.enabled")) {
			File nutchDump = new File(configuration.getString("paths.dump")
					+ "/dump");

			try {
				textDocumentList = TextExtractor.extractNutchDump(nutchDump,
						configuration.getString("paths.extractedDocuments"),
						configuration.getString("extract.extractorClass"));
			} catch (Exception e) {
				e.printStackTrace();
				System.exit(-1);
			}
			if (textDocumentList == null || textDocumentList.isEmpty()) {
				System.out
						.println("Unknown error while extracting: No documents were extracted. Terminating...");
				System.exit(-1);
			}

		}

		if (configuration.getBoolean("addStartends.enabled")) {
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
					e.printStackTrace();
					System.exit(-1);
				}
			}
			for (TextDocument document : textDocumentList) {
				IRSTLMWrapper.addStartEnd(document.getFile(),
						configuration.getString("paths.startendAddedDocuments"));
			}
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

			long startTime, endTime;

			startTime = System.currentTimeMillis();
			KMeans.IRSTLMCluster(textDocumentList,
					configuration.getInt("kmeans.numClusters"));
			endTime = System.currentTimeMillis();
			System.out.println("IRSTLM total execution time: "
					+ (endTime - startTime));
		}

		System.out
				.println("All operations in configuration have been completed.");

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
			System.out
					.println("There seems to be a problem with your \"temp\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.urls").length() == 0) {
			System.out
					.println("There seems to be a problem with your \"urls\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.crawl").length() == 0) {
			System.out
					.println("There seems to be a problem with your \"crawl\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.dump").length() == 0) {
			System.out
					.println("There seems to be a problem with your \"dump\" setting in paths for dumping. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.segments").length() == 0) {
			System.out
					.println("There seems to be a problem with your \"segments\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.extractedDocuments").length() == 0) {
			System.out
					.println("There seems to be a problem with your \"extractedDocuments\" setting in paths for extraction. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.startendAddedDocuments")
				.length() == 0) {
			System.out
					.println("There seems to be a problem with your \"startendAddedDocuments\" setting in paths. Check your config.xml file.");
			return false;
		} else if (configuration.getString("paths.clusterLMs").length() == 0) {
			System.out
					.println("There seems to be a problem with your \"clusterLMs\" setting in paths. Check your config.xml file.");
			return false;
		}

		// Crawl parameters
		if (configuration.getBoolean("crawl.enabled")) {
			if (configuration.getInt("crawl.crawlThreads") < 1) {
				System.out
						.println("There seems to be a problem with your \"crawlThreads\" setting. Check your config.xml file.");
				return false;
			} else if (configuration.getInt("crawl.crawlDepth") < 1) {
				System.out
						.println("There seems to be a problem with your \"crawlDepth\" setting. Check your config.xml file.");
				return false;
			} else if (configuration.getInt("crawl.crawlTopN") < 0) {
				System.out
						.println("There seems to be a problem with your \"crawlTopN\" setting. Check your config.xml file.");
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
				System.out
						.println("You have specified an invalid extractor type "
								+ configuration
										.getString("extract.extractorClass")
								+ " for extractorClass. Possible candidates are ArticleExtractor, CanolaExtractor, DefaultExtractor, LargestContentExtractor and KeepEverythingExtractor.");
				return false;

			}
		}

		if (configuration.getBoolean("addStartends.enabled")) {
		}

		// KMeans settings

		if (configuration.getBoolean("kmeans.enabled")) {
			if (configuration.getInt("kmeans.numClusters") < 1) {
				System.out
						.println("There seems to be a problem with your \"numClusters\" setting. Check your config.xml file.");
				return false;
			}
		}

		return true;

	}
}
