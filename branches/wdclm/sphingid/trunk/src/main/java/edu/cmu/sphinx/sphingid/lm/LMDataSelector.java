package edu.cmu.sphinx.sphingid.lm;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Iterator;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import edu.cmu.sphinx.sphingid.commons.FileUtils;
import edu.cmu.sphinx.sphingid.lm.AbstractLanguageModel.Smoothing;

/**
 * A class for intelligently selecting a subset of data from a general corpus
 * for increasing amount of in-domain data.
 * <p>
 * Implements the paper Moore et.&nbsp;al, Intelligent Selection of Language
 * Model Training Data.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 */
public class LMDataSelector {
	private static final Logger logger = LoggerFactory
			.getLogger(LMDataSelector.class);

	private int n, testSetPercentage;
	private Smoothing smoothing;
	private Charset encoding;
	private File corpusFile, inDomainCorpusFile;
	private File testSet;
	private AbstractLanguageModel corpusModel, inDomainModel;

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
			int testSetPercentage, int n, Smoothing smoothing, Charset encoding)
			throws FileNotFoundException {
		if (corpusFile.exists() && corpusFile.canRead())
			this.corpusFile = corpusFile;
		else
			throw new FileNotFoundException(String.format(
					Messages.getString("LMDataSelector.CannotReadCorpus"), //$NON-NLS-1$
					corpusFile));

		if (inDomainCorpusFile.exists() && inDomainCorpusFile.canRead())
			this.inDomainCorpusFile = inDomainCorpusFile;
		else
			throw new FileNotFoundException(String.format(Messages
					.getString("LMDataSelector.CannotReadInDomainCorpus"), //$NON-NLS-1$
					inDomainCorpusFile));

		this.testSetPercentage = testSetPercentage;

		if (n > 0)
			this.n = n;
		else
			throw new IllegalArgumentException(String.format(
					Messages.getString("LMDataSelector.InvalidNgramValue"), n)); //$NON-NLS-1$

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
	public LMDataSelector(AbstractLanguageModel corpusModel,
			AbstractLanguageModel inDomainModel, File corpus, File testSet,
			int n, Smoothing smoothing, Charset encoding)
			throws FileNotFoundException {
		if (n > 0)
			this.n = n;
		else
			throw new IllegalArgumentException(String.format(
					Messages.getString("LMDataSelector.InvalidNgramValue"), n)); //$NON-NLS-1$

		if (corpus.exists() && corpus.canRead())
			this.corpusFile = corpus;
		else
			throw new FileNotFoundException(String.format(
					Messages.getString("LMDataSelector.CannotReadCorpus"), //$NON-NLS-1$
					corpus));

		this.corpusModel = corpusModel;
		this.inDomainModel = inDomainModel;
		this.smoothing = smoothing;
		this.encoding = encoding;

	}

	public void selectData(int numCutoffSegments, boolean useBuildLmScript)
			throws FileNotFoundException, IOException, InterruptedException {
		if (numCutoffSegments < 2)
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("LMDataSelector.IllegalNumberOfCutoffSegments"), numCutoffSegments)); //$NON-NLS-1$

		BufferedReader corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(this.corpusFile.getPath())),
				this.encoding));

		boolean sentenceMarkers = false;
		String sentenceString = corpusReader.readLine();

		if (sentenceString == null) {
			corpusReader.close();
			throw new IOException(
					Messages.getString("LMDataSelector.CorpusIsEmpty")); //$NON-NLS-1$
		}

		if (sentenceString.equals("</s>")) //$NON-NLS-1$
			sentenceMarkers = true;

		/*
		 * Create test and training sets from in-domain corpus
		 */
		if (this.inDomainModel == null || this.corpusModel == null) {
			logger.info(Messages.getString("LMDataSelector.CreatingSets")); //$NON-NLS-1$
			long startTime = System.currentTimeMillis();
			int numInDomainLines = FileUtils.countLines(
					this.inDomainCorpusFile, this.encoding);
			File trainingSet = new File(this.inDomainCorpusFile + "-training"); //$NON-NLS-1$
			this.testSet = new File(this.inDomainCorpusFile + "-test"); //$NON-NLS-1$
			FileUtils.createTrainingTestSets(this.inDomainCorpusFile,
					trainingSet, this.testSet, false, this.testSetPercentage,
					this.encoding);
			int numTrainingSetLines = FileUtils.countLines(trainingSet,
					this.encoding);
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Extract dictionary from the general corpus
			 */
			logger.info(Messages
					.getString("LMDataSelector.ExtractingDictionary")); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			Dictionary dictionary = new Dictionary(trainingSet, new File(
					trainingSet + ".dict"), 2, -1); //$NON-NLS-1$
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Construct in domain model
			 */
			logger.info(Messages
					.getString("LMDataSelector.ConstructingInDomainModel")); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			if (useBuildLmScript) {

				this.inDomainModel = new GiganticLanguageModel(trainingSet,
						new File(trainingSet + ".giglm"), dictionary, this.n, //$NON-NLS-1$
						this.smoothing, true, sentenceMarkers, 3);
			} else {
				this.inDomainModel = new LanguageModel(
						trainingSet,
						new File(trainingSet + ".lm"), dictionary, this.n, this.smoothing, //$NON-NLS-1$
						true);
			}
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Select the same amount of lines from the general corpus randomly
			 */
			logger.info(
					Messages.getString("LMDataSelector.SelectingLinesFromCorpusRandomly"), numInDomainLines); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			File corpusRandomSubset = new File(this.corpusFile
					+ "-randomsubset-" //$NON-NLS-1$
					+ numTrainingSetLines);
			FileUtils.chooseNRandomLines(this.corpusFile, corpusRandomSubset,
					numTrainingSetLines, this.encoding);
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Construct corpus model
			 */
			logger.info(Messages
					.getString("LMDataSelector.ConstructingCorpusModel")); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			if (useBuildLmScript) {
				this.corpusModel = new GiganticLanguageModel(
						corpusRandomSubset, new File(corpusRandomSubset
								+ ".giglm"), dictionary, //$NON-NLS-1$
						this.n, this.smoothing, true, sentenceMarkers, 3);

			} else {
				this.corpusModel = new LanguageModel(corpusRandomSubset,
						new File(corpusRandomSubset + ".lm"), dictionary, //$NON-NLS-1$
						this.n, this.smoothing, true);
			}
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));
		}

		/*
		 * Get perplexities of sentences of general corpus against general
		 * corpus LM
		 */
		logger.info(Messages
				.getString("LMDataSelector.ComputingSentencePerplexitiesGeneralAgainstGeneral")); //$NON-NLS-1$
		long startTime = System.currentTimeMillis();
		ArrayList<SentencePerplexity> corpusModelSentencePerplexities = this.corpusModel
				.computeSentencePerplexities(this.corpusFile);
		logger.info(
				"{} sentence perplexities obtained against general corpus model.",
				corpusModelSentencePerplexities.size());
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Get perplexities of sentences of general corpus against in-domain
		 * corpus LM
		 */
		logger.info(Messages
				.getString("LMDataSelector.ComputingSentencePerplexitiesGeneralAgainstInDomain")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		ArrayList<SentencePerplexity> inDomainModelSentencePerplexities = this.inDomainModel
				.computeSentencePerplexities(this.corpusFile);
		logger.info(
				"{} sentence perplexities obtained against in-domain corpus model.",
				corpusModelSentencePerplexities.size());
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Calculate perplexity differences
		 */
		ArrayList<SentencePerplexity> perplexityDifferences = new ArrayList<SentencePerplexity>();

		logger.info(Messages
				.getString("LMDataSelector.ComputingPerplexityDifferences")); //$NON-NLS-1$
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

		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Sort perplexity differences
		 */
		logger.info(Messages
				.getString("LMDataSelector.SortingPerplexityDifferences")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareByPerplexity);
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Write perplexity differences to disk for analysis purposes
		 */
		logger.info(Messages
				.getString("LMDataSelector.WritingDifferencesToDisk")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		BufferedWriter pdw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(new File("perplexity-differences")), //$NON-NLS-1$
				this.encoding));

		for (SentencePerplexity perplexity : perplexityDifferences)
			pdw.write(perplexity.getPerplexity()
					+ System.getProperty("line.separator")); //$NON-NLS-1$

		pdw.close();
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Divide to cutoff parts
		 */
		int numSentences = perplexityDifferences.size();
		logger.info(
				Messages.getString("LMDataSelector.NumPerplexityDifferences"), numSentences); //$NON-NLS-1$

		logger.info(Messages
				.getString("LMDataSelector.GeneratingCutoffSegments")); //$NON-NLS-1$
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
			logger.info(Messages
					.getString("LMDataSelector.ChoosingThresholdForSegment"), //$NON-NLS-1$
					thresholds[i], i);

			File segment = new File(this.corpusFile + ".segment" + i + ".txt"); //$NON-NLS-1$ //$NON-NLS-2$
			segments[i] = segment;
			BufferedWriter bw = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(segment), this.encoding));
			segmentWriters[i] = bw;

			File randomSegment = new File(this.corpusFile + ".randsegment" + i //$NON-NLS-1$
					+ ".txt"); //$NON-NLS-1$
			randomSegments[i] = randomSegment;
			BufferedWriter rbw = new BufferedWriter(new OutputStreamWriter(
					new FileOutputStream(randomSegment), this.encoding));
			randomWriters[i] = rbw;
		}

		/*
		 * Create a random selection list TODO: A bit ugly.
		 */
		logger.info(Messages
				.getString("LMDataSelector.CreatingRandomSelectionList")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		IntegerArray intArray = new IntegerArray(numSentences);
		for (int i = 0; i < numSentences; ++i)
			intArray.put(i, i);

		intArray.shuffle();
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Sort perplexity differences according to sentence number, since we
		 * need to read the file sequentially
		 */
		logger.info(Messages
				.getString("LMDataSelector.SortingPerplexityDifferences")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareBySentenceNumber);
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Read and write all segments in one pass
		 */
		logger.info(Messages.getString("LMDataSelector.WritingSegmentsToDisk")); //$NON-NLS-1$

		corpusReader.close();
		corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(this.corpusFile.getPath())),
				this.encoding));

		if (sentenceMarkers) {
			for (int i = 1; i < numCutoffSegments; ++i) {
				segmentWriters[i].write("</s>" //$NON-NLS-1$
						+ System.getProperty("line.separator")); //$NON-NLS-1$
				randomWriters[i].write("</s>" //$NON-NLS-1$
						+ System.getProperty("line.separator")); //$NON-NLS-1$
			}
		}

		for (int i = 0; i < perplexityDifferences.size(); ++i) {
			SentencePerplexity sentence = perplexityDifferences.get(i);
			sentenceString = corpusReader.readLine();

			if (sentenceMarkers)
				if (sentenceString.trim().equalsIgnoreCase("<s>") //$NON-NLS-1$
						|| sentenceString.trim().equalsIgnoreCase("</s>")) //$NON-NLS-1$
					continue;

			/*
			 * Write sentences to their corresponding segments
			 */
			for (int j = numCutoffSegments - 1; j > 0; --j) {
				if (sentence.getPerplexity() < thresholds[j]) {
					segmentWriters[j].write(sentenceString
							+ System.getProperty("line.separator")); //$NON-NLS-1$
				} else
					break;
			}

			/*
			 * Write to random selection segments
			 */
			for (int j = numCutoffSegments - 1; j > 0; --j) {
				if (intArray.get(i) < j * sentencesPerSegment) {
					randomWriters[j].write(sentenceString
							+ System.getProperty("line.separator")); //$NON-NLS-1$
				} else
					break;
			}
		}

		perplexityDifferences.clear();
		intArray = null;

		if (sentenceMarkers) {
			for (int i = 1; i < numCutoffSegments; ++i) {
				segmentWriters[i].write("<s>"); //$NON-NLS-1$
				randomWriters[i].write("<s>"); //$NON-NLS-1$
			}
		}

		for (int i = 1; i < numCutoffSegments; ++i) {
			segmentWriters[i].close();
			randomWriters[i].close();
		}
		logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Construct LMs for each segment and compute perplexities
		 */
		logger.info(Messages
				.getString("LMDataSelector.ConstructingLanguageModelsForSegments")); //$NON-NLS-1$
		for (int i = 1; i < numCutoffSegments; ++i) {
			AbstractLanguageModel lm = null;
			logger.info("Constructing language model for segment {}...", i);
			if (useBuildLmScript) {
				lm = new GiganticLanguageModel(segments[i], new File(
						segments[i] + ".giglm"), null, this.n, //$NON-NLS-1$
						this.smoothing, false, sentenceMarkers, 3);
			} else {
				lm = new LanguageModel(segments[i], new File(segments[i]
						+ ".lm"), null, this.n, this.smoothing, true); //$NON-NLS-1$
			}
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			logger.info(
					Messages.getString("LMDataSelector.PerplexityOfSegment"), i, //$NON-NLS-1$
					lm.computePerplexity(this.testSet));

			logger.info("Constructing language model for random segment {}...",
					i);
			if (useBuildLmScript) {
				lm = new GiganticLanguageModel(randomSegments[i], new File(
						randomSegments[i] + ".giglm"), null, this.n, //$NON-NLS-1$
						this.smoothing, false, sentenceMarkers, 3);
			} else {
				lm = new LanguageModel(randomSegments[i], new File(
						randomSegments[i] + ".lm"), null, this.n, //$NON-NLS-1$
						this.smoothing, true);
			}
			logger.info(Messages.getString("LMDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			logger.info(Messages
					.getString("LMDataSelector.PerplexityOfRandomSegment"), //$NON-NLS-1$
					i, lm.computePerplexity(this.testSet));

		}
	}
}
