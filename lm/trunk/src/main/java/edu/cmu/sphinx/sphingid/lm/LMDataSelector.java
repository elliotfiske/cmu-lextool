package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStreamWriter;

import java.nio.charset.Charset;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A class for intelligently selecting a subset of data from a general corpus
 * for increasing amount of in-domain data.
 * <p>
 * Implements the paper Moore et.&nbsp;al, Intelligent Selection of Language
 * Model Training Data.
 * 
 * @author Emre Çelikten
 */
public class LMDataSelector {
	private static final Logger logger = LoggerFactory
			.getLogger(LMDataSelector.class);

	private int n, testSetPercentage;
	private String smoothing;
	private Charset encoding;
	private File corpusFile, inDomainCorpusFile;
	private File testSet;
	private GiganticLanguageModel corpusModel, inDomainModel;

	/**
	 * Creates a LMDataSelector to use without any existing language models.
	 * 
	 * @param corpusFile
	 *            the corpus that will be used in training a general language
	 *            model
	 * @param inDomainCorpusFile
	 *            the corpus that will be used in training a in-domain language
	 *            model
	 * @param testSetPercentage
	 *            amount of in-domain data to be used as test set
	 * @param n
	 *            number of tokens in each n-gram
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param encoding
	 *            {@link Charset} object that specifies encoding to use when
	 *            dealing with files
	 * 
	 * @throws FileNotFoundException
	 *             if corpusFile or inDomainCorpusFile cannot be read
	 */
	public LMDataSelector(File corpusFile, File inDomainCorpusFile,
			int testSetPercentage, int n, String smoothing, Charset encoding)
			throws FileNotFoundException {
		if (corpusFile.exists() && corpusFile.canRead())
			this.corpusFile = corpusFile;
		else
			throw new FileNotFoundException(
					"Cannot read corpus file "
							+ corpusFile
							+ ". Check if the file exists or permissions are set correctly.");

		if (inDomainCorpusFile.exists() && inDomainCorpusFile.canRead())
			this.inDomainCorpusFile = inDomainCorpusFile;
		else
			throw new FileNotFoundException(
					"Cannot read corpus file "
							+ inDomainCorpusFile
							+ ". Check if the file exists or permissions are set correctly.");

		this.testSetPercentage = testSetPercentage;

		if (n > 0)
			this.n = n;
		else
			throw new IllegalArgumentException("Invalid n-gram value of " + n
					+ ".");

		this.smoothing = smoothing;
		this.encoding = encoding;
	}

	/**
	 * Creates a LMDataSelector to use with existing language models.
	 * 
	 * @param corpusModel
	 *            language model file for a subset of general corpus
	 * @param inDomainModel
	 *            language model file for in-domain corpus
	 * @param corpus
	 *            the pointer to general corpus file
	 * @param testSet
	 *            a segment of text from in-domain corpus to be used for
	 *            computing perplexities
	 * @param n
	 *            number of tokens in each n-gram
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param encoding
	 *            {@link Charset} object that specifies encoding to use when
	 *            dealing with files
	 * 
	 * @throws FileNotFoundException
	 *             if the files do not exist or cannot be read
	 */
	public LMDataSelector(GiganticLanguageModel corpusModel,
			GiganticLanguageModel inDomainModel, File corpus, File testSet,
			int n, String smoothing, Charset encoding)
			throws FileNotFoundException {
		if (n > 0)
			this.n = n;
		else
			throw new IllegalArgumentException("Invalid n-gram value of " + n
					+ ".");

		if (corpus.exists() && corpus.canRead())
			this.corpusFile = corpus;
		else
			throw new FileNotFoundException(
					"Cannot read corpus file "
							+ corpus
							+ ". Check if the file exists or permissions are set correctly.");

		this.corpusModel = corpusModel;
		this.inDomainModel = inDomainModel;
		this.smoothing = smoothing;
		this.encoding = encoding;

	}

	public void selectData(int numCutoffSegments) throws FileNotFoundException,
			IOException, InterruptedException {
		if (numCutoffSegments < 2)
			throw new IllegalArgumentException(
					"Illegal number of cutoff segments");

		BufferedReader corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(corpusFile.getPath())), encoding));

		boolean sentenceMarkers = false;
		String sentenceString = corpusReader.readLine();

		if (sentenceString.equals("</s>"))
			sentenceMarkers = true;

		/*
		 * Create test and training sets from in-domain corpus
		 */
		if (inDomainModel == null && corpusModel == null) {
			logger.info("Creating training and test sets...");
			long startTime = System.currentTimeMillis();
			int numInDomainLines = FileUtils.countLines(inDomainCorpusFile,
					encoding);
			File trainingSet = new File(inDomainCorpusFile + "-training");
			this.testSet = new File(inDomainCorpusFile + "-test");
			FileUtils.createTrainingTestSets(inDomainCorpusFile, trainingSet,
					this.testSet, false, this.testSetPercentage, encoding);
			int numTrainingSetLines = FileUtils.countLines(trainingSet,
					encoding);
			logger.info("Operation completed in "
					+ ((System.currentTimeMillis() - startTime) / 1000)
					+ " seconds.");

			/*
			 * Construct LMs from corpus and in domain data
			 */
			logger.info("Extracting dictionary from training corpus...");
			startTime = System.currentTimeMillis();
			Dictionary dictionary = new Dictionary(trainingSet,
					new File(trainingSet + ".dict"), 2, -1);
			logger.info("Operation completed in "
					+ ((System.currentTimeMillis() - startTime) / 1000)
					+ " seconds.");

			logger.info("Constructing in-domain model...");
			startTime = System.currentTimeMillis();
			this.inDomainModel = new GiganticLanguageModel(trainingSet,
					new File(trainingSet + ".giglm"), this.n, this.smoothing,
					true, sentenceMarkers, dictionary, 3);
			logger.info("Operation completed in "
					+ ((System.currentTimeMillis() - startTime) / 1000)
					+ " seconds.");

			logger.info("Selecting " + numInDomainLines
					+ " lines from the main corpus randomly...");
			startTime = System.currentTimeMillis();
			File corpusRandomSubset = new File(corpusFile + "-randomsubset-"
					+ numTrainingSetLines);
			FileUtils.chooseNRandomLines(corpusFile, corpusRandomSubset,
					numTrainingSetLines, encoding);
			logger.info("Operation completed in "
					+ ((System.currentTimeMillis() - startTime) / 1000)
					+ " seconds.");

			logger.info("Constructing corpus model...");
			startTime = System.currentTimeMillis();
			this.corpusModel = new GiganticLanguageModel(corpusRandomSubset,
					new File(corpusRandomSubset + ".giglm"), this.n,
					this.smoothing, true, sentenceMarkers, dictionary, 3);
			logger.info("Operation completed in "
					+ ((System.currentTimeMillis() - startTime) / 1000)
					+ " seconds.");
		}

		/*
		 * Get perplexities of sentences of big corpus against big corpus LM
		 */
		logger.info("Computing perplexities of sentences: Big corpus against big corpus LM...");
		long startTime = System.currentTimeMillis();
		ArrayList<SentencePerplexity> corpusModelSentencePerplexities = corpusModel
				.computeSentencePerplexities(corpusFile);
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Get perplexities of sentences of big corpus against in-domain corpus
		 * LM
		 */
		logger.info("Computing perplexities of sentences: Big corpus against in-domain corpus LM...");
		startTime = System.currentTimeMillis();
		ArrayList<SentencePerplexity> inDomainModelSentencePerplexities = inDomainModel
				.computeSentencePerplexities(corpusFile);
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Calculate perplexity differences for the main corpus
		 */
		ArrayList<SentencePerplexity> perplexityDifferences = new ArrayList<SentencePerplexity>();

		logger.info("Computing perplexity differences...");
		startTime = System.currentTimeMillis();
		for (Iterator<SentencePerplexity> corpusIterator = corpusModelSentencePerplexities
				.iterator(), inDomainIterator = inDomainModelSentencePerplexities
				.iterator(); corpusIterator.hasNext()
				&& inDomainIterator.hasNext();) {
			SentencePerplexity corpusSentence = corpusIterator.next();
			SentencePerplexity inDomainSentence = inDomainIterator.next();

			perplexityDifferences.add(new SentencePerplexity(corpusSentence
					.getSentenceNumber(), inDomainSentence.getPerplexity()
					- corpusSentence.getPerplexity()));
		}

		corpusModelSentencePerplexities.clear();
		inDomainModelSentencePerplexities.clear();

		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Sort perplexity differences
		 */
		logger.info("Sorting perplexity differences...");
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareByPerplexity);
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Write perplexity differences to disk for analysis purposes
		 */
		logger.info("Writing perplexity differences to disk...");
		startTime = System.currentTimeMillis();
		BufferedWriter pdw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(new File("perplexity-differences")),
				encoding));

		for (SentencePerplexity perplexity : perplexityDifferences)
			pdw.write(perplexity.getPerplexity()
					+ System.getProperty("line.separator"));

		pdw.close();
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Divide to cutoff parts
		 */
		int numSentences = perplexityDifferences.size();
		logger.info("There are " + numSentences + " perplexity differences.");

		logger.info("Generating cutoff segments...");
		float thresholds[] = new float[numCutoffSegments];
		File segments[] = new File[numCutoffSegments];
		File randomSegments[] = new File[numCutoffSegments];
		BufferedWriter segmentWriters[] = new BufferedWriter[numCutoffSegments];
		BufferedWriter randomWriters[] = new BufferedWriter[numCutoffSegments];
		int sentencesPerSegment = (int) Math.floor(numSentences
				/ numCutoffSegments);
		for (int i = 1; i < numCutoffSegments; ++i) {
			thresholds[i] = perplexityDifferences.get(i * sentencesPerSegment)
					.getPerplexity();
			logger.info("Choosing threshold " + thresholds[i] + " for segment "
					+ i + ".");

			File segment = new File(corpusFile + ".segment" + i + ".txt");
			segments[i] = segment;
			BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(segment), encoding));
			segmentWriters[i] = bw;

			File randomSegment = new File(corpusFile + ".randsegment" + i
					+ ".txt");
			randomSegments[i] = randomSegment;
			BufferedWriter rbw = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(randomSegment), encoding));
			randomWriters[i] = rbw;
		}

		/*
		 * Create a random selection list TODO: A bit ugly.
		 */
		logger.info("Creating a random selection list...");
		startTime = System.currentTimeMillis();
		IntegerArray intArray = new IntegerArray(numSentences);
		for (int i = 0; i < numSentences; ++i)
			intArray.put(i, i);

		intArray.shuffle();
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Sort perplexity differences according to sentence number, since we
		 * need to read the file sequentially
		 */
		logger.info("Sorting perplexity differences...");
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareBySentenceNumber);
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Read and write all segments in one pass
		 */
		logger.info("Writing segments to disk...");

		corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(corpusFile.getPath())), encoding));

		if (sentenceMarkers) {
			for (int i = 1; i < numCutoffSegments; ++i) {
				segmentWriters[i].write("</s>"
						+ System.getProperty("line.separator"));
				randomWriters[i].write("</s>"
						+ System.getProperty("line.separator"));
			}
		}

		for (int i = 0; i < perplexityDifferences.size(); ++i) {
			SentencePerplexity sentence = perplexityDifferences.get(i);
			sentenceString = corpusReader.readLine();

			if (sentenceMarkers)
				if (sentenceString.trim().equalsIgnoreCase("<s>")
						|| sentenceString.trim().equalsIgnoreCase("</s>"))
					continue;

			/*
			 * Write sentences to their corresponding segments
			 */
			for (int j = numCutoffSegments - 1; j > 0; --j) {
				if (sentence.getPerplexity() < thresholds[j]) {
					segmentWriters[j].write(sentenceString
							+ System.getProperty("line.separator"));
				} else
					break;
			}

			/*
			 * Write to random selection segments
			 */
			for (int j = numCutoffSegments - 1; j > 0; --j) {
				if (intArray.get(i) < j * sentencesPerSegment) {
					randomWriters[j].write(sentenceString
							+ System.getProperty("line.separator"));
				} else
					break;
			}
		}

		perplexityDifferences.clear();
		intArray = null;

		if (sentenceMarkers) {
			for (int i = 1; i < numCutoffSegments; ++i) {
				segmentWriters[i].write("<s>");
				randomWriters[i].write("<s>");
			}
		}

		for (int i = 1; i < numCutoffSegments; ++i) {
			segmentWriters[i].close();
			randomWriters[i].close();
		}
		logger.info("Operation completed in "
				+ ((System.currentTimeMillis() - startTime) / 1000)
				+ " seconds.");

		/*
		 * Construct LMs for each segment and compute perplexities
		 */
		for (int i = 1; i < numCutoffSegments; ++i) {
			GiganticLanguageModel glm = new GiganticLanguageModel(segments[i],
					new File(segments[i] + ".giglm"), this.n, this.smoothing,
					false, sentenceMarkers, null, 3);
			logger.info("Perplexity of test set for segment " + i + " is "
					+ glm.computePerplexity(testSet) + ".");
			glm = new GiganticLanguageModel(randomSegments[i], new File(
					randomSegments[i] + ".giglm"), this.n, this.smoothing,
					false, sentenceMarkers, null, 3);
			logger.info("Perplexity of test set for random segment " + i
					+ " is " + glm.computePerplexity(testSet) + ".");
		}
	}
}

