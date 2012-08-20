package edu.cmu.sphinx.sphingid.dataselection;

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
import edu.cmu.sphinx.sphingid.commons.MathUtils;
import edu.cmu.sphinx.sphingid.lm.AbstractLanguageModel;
import edu.cmu.sphinx.sphingid.lm.AbstractLanguageModel.Smoothing;
import edu.cmu.sphinx.sphingid.lm.Dictionary;
import edu.cmu.sphinx.sphingid.lm.GiganticLanguageModel;
import edu.cmu.sphinx.sphingid.lm.IntegerArray;
import edu.cmu.sphinx.sphingid.lm.LanguageModel;
import edu.cmu.sphinx.sphingid.lm.SentencePerplexity;

/**
 * A class for intelligently selecting a subset of data from a general corpus
 * for increasing amount of in-domain data.
 * <p>
 * Implements the paper Moore et.&nbsp;al, Intelligent Selection of Language
 * Model Training Data.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 */
public class PerplexityBasedDataSelector implements DataSelector {
	private static final Logger logger = LoggerFactory
			.getLogger(PerplexityBasedDataSelector.class);

	private int n, testSetPercentage, numCutoffSegments;
	private Smoothing smoothing;
	private Charset encoding;
	private File corpusFile, inDomainCorpusFile;
	private File testSet;
	private AbstractLanguageModel corpusModel, inDomainModel;
	private boolean useBuildLmScript;

	/**
	 * Creates a PerplexityBasedDataSelector to use without any existing
	 * language models.
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
	 * 
	 * @param smoothing
	 *            smoothing type to be used in language model
	 * @param numCutoffSegments
	 *            number of perplexity cutoff segments that the corpus will be
	 *            divided into
	 * @param encoding
	 *            {@link Charset} object that specifies encoding to use when
	 *            dealing with files
	 * @param useBuildLmScript
	 *            boolean variable to choose if the selector will use tlm or
	 *            build-lm.sh of IRST LM
	 * 
	 * @throws FileNotFoundException
	 *             if corpusFile or inDomainCorpusFile cannot be read
	 */
	public PerplexityBasedDataSelector(File corpusFile,
			File inDomainCorpusFile, int testSetPercentage, int n,
			Smoothing smoothing, int numCutoffSegments, Charset encoding,
			boolean useBuildLmScript) throws FileNotFoundException {
		if (corpusFile.exists() && corpusFile.canRead())
			this.corpusFile = corpusFile;
		else
			throw new FileNotFoundException(String.format(Messages
					.getString("PerplexityBasedDataSelector.CannotReadCorpus"), //$NON-NLS-1$
					corpusFile));

		if (inDomainCorpusFile.exists() && inDomainCorpusFile.canRead())
			this.inDomainCorpusFile = inDomainCorpusFile;
		else
			throw new FileNotFoundException(
					String.format(
							Messages.getString("PerplexityBasedDataSelector.CannotReadInDomainCorpus"), //$NON-NLS-1$
							inDomainCorpusFile));

		this.testSetPercentage = testSetPercentage;

		if (n > 0)
			this.n = n;
		else
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("PerplexityBasedDataSelector.InvalidNgramValue"), n)); //$NON-NLS-1$

		this.smoothing = smoothing;

		if (numCutoffSegments < 2)
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("PerplexityBasedDataSelector.IllegalNumberOfCutoffSegments"), numCutoffSegments)); //$NON-NLS-1$
		
		this.numCutoffSegments = numCutoffSegments;

		this.encoding = encoding;
		this.useBuildLmScript = useBuildLmScript;
	}

	/**
	 * Creates a PerplexityBasedDataSelector to use with existing language
	 * models.
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
	public PerplexityBasedDataSelector(AbstractLanguageModel corpusModel,
			AbstractLanguageModel inDomainModel, File corpus, File testSet,
			int n, Smoothing smoothing, int numCutoffSegments,
			Charset encoding, boolean useBuildLmScript)
			throws FileNotFoundException {
		if (n > 0)
			this.n = n;
		else
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("PerplexityBasedDataSelector.InvalidNgramValue"), n)); //$NON-NLS-1$

		if (corpus.exists() && corpus.canRead())
			this.corpusFile = corpus;
		else
			throw new FileNotFoundException(String.format(Messages
					.getString("PerplexityBasedDataSelector.CannotReadCorpus"), //$NON-NLS-1$
					corpus));

		this.corpusModel = corpusModel;
		this.inDomainModel = inDomainModel;
		this.smoothing = smoothing;

		if (numCutoffSegments < 2)
			throw new IllegalArgumentException(
					String.format(
							Messages.getString("PerplexityBasedDataSelector.IllegalNumberOfCutoffSegments"), numCutoffSegments)); //$NON-NLS-1$

		this.encoding = encoding;
		this.useBuildLmScript = useBuildLmScript;
	}

	@Override
	public void selectData() throws IOException, InterruptedException {

		BufferedReader corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(this.corpusFile.getPath())),
				this.encoding));

		boolean sentenceMarkers = false;
		String sentenceString = corpusReader.readLine();

		if (sentenceString == null) {
			corpusReader.close();
			throw new IOException(
					Messages.getString("PerplexityBasedDataSelector.CorpusIsEmpty")); //$NON-NLS-1$
		}

		if (sentenceString.equals("</s>")) //$NON-NLS-1$
			sentenceMarkers = true;

		/*
		 * Create test and training sets from in-domain corpus
		 */
		if (this.inDomainModel == null || this.corpusModel == null) {
			logger.info(Messages
					.getString("PerplexityBasedDataSelector.CreatingSets")); //$NON-NLS-1$
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
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Extract dictionary from the general corpus
			 */
			logger.info(Messages
					.getString("PerplexityBasedDataSelector.ExtractingDictionary")); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			Dictionary dictionary = new Dictionary(trainingSet, new File(
					trainingSet + ".dict"), 2, -1); //$NON-NLS-1$
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Construct in domain model
			 */
			logger.info(Messages
					.getString("PerplexityBasedDataSelector.ConstructingInDomainModel")); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			if (this.useBuildLmScript) {

				this.inDomainModel = new GiganticLanguageModel(trainingSet,
						new File(trainingSet + ".giglm"), dictionary, this.n, //$NON-NLS-1$
						this.smoothing, true, sentenceMarkers, 3);
			} else {
				this.inDomainModel = new LanguageModel(
						trainingSet,
						new File(trainingSet + ".lm"), dictionary, this.n, this.smoothing, //$NON-NLS-1$
						true);
			}
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Select the same amount of lines from the general corpus randomly
			 */
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.SelectingLinesFromCorpusRandomly"), numInDomainLines); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			File corpusRandomSubset = new File(this.corpusFile
					+ "-randomsubset-" //$NON-NLS-1$
					+ numTrainingSetLines);
			FileUtils.chooseNRandomLines(this.corpusFile, corpusRandomSubset,
					numTrainingSetLines, this.encoding);
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			/*
			 * Construct corpus model
			 */
			logger.info(Messages
					.getString("PerplexityBasedDataSelector.ConstructingCorpusModel")); //$NON-NLS-1$
			startTime = System.currentTimeMillis();
			if (this.useBuildLmScript) {
				this.corpusModel = new GiganticLanguageModel(
						corpusRandomSubset, new File(corpusRandomSubset
								+ ".giglm"), dictionary, //$NON-NLS-1$
						this.n, this.smoothing, true, sentenceMarkers, 3);

			} else {
				this.corpusModel = new LanguageModel(corpusRandomSubset,
						new File(corpusRandomSubset + ".lm"), dictionary, //$NON-NLS-1$
						this.n, this.smoothing, true);
			}
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));
		}

		/*
		 * Get perplexities of sentences of general corpus against general
		 * corpus LM
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.ComputingSentencePerplexitiesGeneralAgainstGeneral")); //$NON-NLS-1$
		long startTime = System.currentTimeMillis();
		ArrayList<SentencePerplexity> corpusModelSentencePerplexities = this.corpusModel
				.computeSentencePerplexities(this.corpusFile);
		logger.info(
				Messages.getString("PerplexityBasedDataSelector.NumSentencePerplexitiesObtainedAgainstGeneralModel"), //$NON-NLS-1$
				corpusModelSentencePerplexities.size());
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Get perplexities of sentences of general corpus against in-domain
		 * corpus LM
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.ComputingSentencePerplexitiesGeneralAgainstInDomain")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		ArrayList<SentencePerplexity> inDomainModelSentencePerplexities = this.inDomainModel
				.computeSentencePerplexities(this.corpusFile);
		logger.info(
				Messages.getString("PerplexityBasedDataSelector.NumSentencePerplexitiesObtainedAgainstInDomainModel"), //$NON-NLS-1$
				corpusModelSentencePerplexities.size());
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Calculate perplexity differences
		 */
		ArrayList<SentencePerplexity> perplexityDifferences = new ArrayList<SentencePerplexity>();

		logger.info(Messages
				.getString("PerplexityBasedDataSelector.ComputingPerplexityDifferences")); //$NON-NLS-1$
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

		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Sort perplexity differences
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.SortingPerplexityDifferences")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareByPerplexity);
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Write perplexity differences to disk for analysis purposes
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.WritingDifferencesToDisk")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		BufferedWriter pdw = new BufferedWriter(new OutputStreamWriter(
				new FileOutputStream(new File("perplexity-differences")), //$NON-NLS-1$
				this.encoding));

		for (SentencePerplexity perplexity : perplexityDifferences)
			pdw.write(perplexity.getPerplexity()
					+ System.getProperty("line.separator")); //$NON-NLS-1$

		pdw.close();
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Divide to cutoff parts
		 */
		int numSentences = perplexityDifferences.size();
		logger.info(
				Messages.getString("PerplexityBasedDataSelector.NumPerplexityDifferences"), numSentences); //$NON-NLS-1$

		logger.info(Messages
				.getString("PerplexityBasedDataSelector.GeneratingCutoffSegments")); //$NON-NLS-1$
		float thresholds[] = new float[this.numCutoffSegments];
		File segments[] = new File[this.numCutoffSegments];
		File randomSegments[] = new File[this.numCutoffSegments];
		BufferedWriter segmentWriters[] = new BufferedWriter[this.numCutoffSegments];
		BufferedWriter randomWriters[] = new BufferedWriter[this.numCutoffSegments];
		int sentencesPerSegment = (int) Math.floor(numSentences
				/ this.numCutoffSegments);
		for (int i = 1; i < this.numCutoffSegments; ++i) {
			thresholds[i] = perplexityDifferences.get(i * sentencesPerSegment)
					.getPerplexity();
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.ChoosingThresholdForSegment"), //$NON-NLS-1$
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
				.getString("PerplexityBasedDataSelector.CreatingRandomSelectionList")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		IntegerArray intArray = new IntegerArray(numSentences);
		for (int i = 0; i < numSentences; ++i)
			intArray.put(i, i);

		intArray.shuffle();
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Sort perplexity differences according to sentence number, since we
		 * need to read the file sequentially
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.SortingPerplexityDifferences")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareBySentenceNumber);
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Read and write all segments in one pass
		 */
		startTime = System.currentTimeMillis();
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.WritingSegmentsToDisk")); //$NON-NLS-1$

		corpusReader.close();
		corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(this.corpusFile.getPath())),
				this.encoding));

		if (sentenceMarkers) {
			for (int i = 1; i < this.numCutoffSegments; ++i) {
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
			for (int j = this.numCutoffSegments - 1; j > 0; --j) {
				if (sentence.getPerplexity() < thresholds[j]) {
					segmentWriters[j].write(sentenceString
							+ System.getProperty("line.separator")); //$NON-NLS-1$
				} else
					break;
			}

			/*
			 * Write to random selection segments
			 */
			for (int j = this.numCutoffSegments - 1; j > 0; --j) {
				if (intArray.get(i) < j * sentencesPerSegment) {
					randomWriters[j].write(sentenceString
							+ System.getProperty("line.separator")); //$NON-NLS-1$
				} else
					break;
			}
		}

		intArray = null;

		if (sentenceMarkers) {
			for (int i = 1; i < this.numCutoffSegments; ++i) {
				segmentWriters[i].write("<s>"); //$NON-NLS-1$
				randomWriters[i].write("<s>"); //$NON-NLS-1$
			}
		}

		for (int i = 1; i < this.numCutoffSegments; ++i) {
			segmentWriters[i].close();
			randomWriters[i].close();
		}
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Construct LMs for each segment and compute perplexities
		 */

		float[] perplexities = new float[this.numCutoffSegments];

		logger.info(Messages
				.getString("PerplexityBasedDataSelector.ConstructingLanguageModelsForSegments")); //$NON-NLS-1$
		for (int i = 1; i < this.numCutoffSegments; ++i) {
			AbstractLanguageModel lm = null;
			startTime = System.currentTimeMillis();
			logger.info(Messages
					.getString("PerplexityBasedDataSelector.ConstructingSegmentModel"), i); //$NON-NLS-1$
			if (this.useBuildLmScript) {
				lm = new GiganticLanguageModel(segments[i], new File(
						segments[i] + ".giglm"), null, this.n, //$NON-NLS-1$
						this.smoothing, false, sentenceMarkers, 3);
			} else {
				lm = new LanguageModel(segments[i], new File(segments[i]
						+ ".lm"), null, this.n, this.smoothing, true); //$NON-NLS-1$
			}
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			perplexities[i] = lm.computePerplexity(this.testSet);
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.PerplexityOfSegment"), i, //$NON-NLS-1$
					perplexities[i]);

			startTime = System.currentTimeMillis();
			logger.info(Messages.getString("PerplexityBasedDataSelector.ConstructingRandomSegmentModel"), //$NON-NLS-1$
					i);
			if (this.useBuildLmScript) {
				lm = new GiganticLanguageModel(randomSegments[i], new File(
						randomSegments[i] + ".giglm"), null, this.n, //$NON-NLS-1$
						this.smoothing, false, sentenceMarkers, 3);
			} else {
				lm = new LanguageModel(randomSegments[i], new File(
						randomSegments[i] + ".lm"), null, this.n, //$NON-NLS-1$
						this.smoothing, true);
			}
			logger.info(
					Messages.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
					, ((System.currentTimeMillis() - startTime) / 1000));

			logger.info(
					Messages.getString("PerplexityBasedDataSelector.PerplexityOfRandomSegment"), //$NON-NLS-1$
					i, lm.computePerplexity(this.testSet));

		}

		/*
		 * Calculate optimum number of sentences
		 */

		startTime = System.currentTimeMillis();
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.FittingPolynomial")); //$NON-NLS-1$
		int optimum = MathUtils.findOptimumThreshold(perplexities,
				sentencesPerSegment, 6);
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		if (optimum < sentencesPerSegment) {
			logger.info("Optimum number of sentences are smaller than the number of sentences in the smallest LM! You might want to use smaller LMs. Terminating...");
			return;
		} else if (optimum > sentencesPerSegment * numCutoffSegments) {
			logger.info("Optimum number of sentences are greater than the number of sentences in the biggest LM! Terminating...");
			return;
		}

		logger.info(Messages
				.getString("PerplexityBasedDataSelector.NumSentencesForLowestPerplexity")); //$NON-NLS-1$

		/*
		 * Sort perplexity differences
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.SortingPerplexityDifferences")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareByPerplexity);
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Selecting optimum perplexity threshold
		 */
		float optimumThreshold = perplexityDifferences.get(optimum)
				.getPerplexity();
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.ChoosingThresholdForOptimumSegment")); //$NON-NLS-1$

		/*
		 * Sort perplexity differences according to sentence number, since we
		 * need to read the file sequentially
		 */
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.SortingPerplexityDifferences")); //$NON-NLS-1$
		startTime = System.currentTimeMillis();
		Collections.sort(perplexityDifferences,
				SentencePerplexity.SentenceComparator.compareBySentenceNumber);
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		/*
		 * Write optimum segment file to disk
		 */
		corpusReader = new BufferedReader(new InputStreamReader(
				new FileInputStream(new File(this.corpusFile.getPath())),
				this.encoding));

		File optimumSegment = new File(this.corpusFile + ".optimumsegment" //$NON-NLS-1$
				+ ".txt"); //$NON-NLS-1$

		BufferedWriter optimumWriter = new BufferedWriter(
				new OutputStreamWriter(new FileOutputStream(optimumSegment),
						this.encoding));

		if (sentenceMarkers) {
			for (int i = 1; i < this.numCutoffSegments; ++i) {
				optimumWriter.write("</s>" //$NON-NLS-1$
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

			if (sentence.getPerplexity() < optimumThreshold) {
				optimumWriter.write(sentenceString
						+ System.getProperty("line.separator")); //$NON-NLS-1$
			} else
				break;
		}
		
		optimumWriter.close();
		corpusReader.close();

		/*
		 * Construct LM for the optimum segment
		 */
		AbstractLanguageModel lm = null;
		startTime = System.currentTimeMillis();
		logger.info(Messages.getString("PerplexityBasedDataSelector.ConstructingOptimumModel")); //$NON-NLS-1$
		if (this.useBuildLmScript) {
			lm = new GiganticLanguageModel(optimumSegment, new File(
					optimumSegment + ".giglm"), null, this.n, //$NON-NLS-1$
					this.smoothing, false, sentenceMarkers, 3);
		} else {
			lm = new LanguageModel(optimumSegment, new File(optimumSegment
					+ ".lm"), null, this.n, this.smoothing, true); //$NON-NLS-1$
		}
		logger.info(Messages
				.getString("PerplexityBasedDataSelector.OperationCompleted") //$NON-NLS-1$
				, ((System.currentTimeMillis() - startTime) / 1000));

		double optimumPerplexity = lm.computePerplexity(this.testSet);
		logger.info(
				Messages.getString("PerplexityBasedDataSelector.PerplexityOfOptimumSegment"), //$NON-NLS-1$
				optimumPerplexity);

	}
}
