/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.ArrayList;

import org.apache.commons.configuration.XMLConfiguration;
import org.apache.commons.lang.exception.ExceptionUtils;
import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.entity.ContentType;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import de.l3s.boilerpipe.BoilerpipeProcessingException;
import edu.cmu.sphinx.sphingid.commons.FileUtils;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings;

/**
 * A class that can crawl a website and extract text from it.
 * 
 * @author Emre Çelikten
 * 
 */
public class CrawlerRunnable implements Runnable {
	private static final Logger logger = LoggerFactory
			.getLogger(CrawlerRunnable.class);

	static void removeDisallowedAndOutgoing(ArrayList<String> links, Host host) {
		RobotSettings robotSettings = host.getRobotSettings();
		for (int i = 0; i < links.size(); i++) {
			String link = links.get(i);
			if (!robotSettings.accepts(link)) {
				links.remove(i);
				i--;
			}
		}
	}

	private transient Thread associatedThread;
	private int connectionTimeout, readTimeout;
	private String crawlerName;
	private volatile long fetchDurationHistory[];
	private Host[] hosts;
	private volatile int lastHistoryPos;
	private volatile LinkQueue[] linkQueues;
	private volatile float movingWordAverage, fetchDurationAverage,
			wordsPerSecond;
	private volatile long numTotalWords, numTotalPages;
	private volatile int numWordsHistory[];
	private long averageCrawlDelay;
	private volatile PageTable pageTable;
	private UrlTable urlTable;
	transient HttpClient httpClient;
	private FetchScheduler fetchScheduler;
	private RecrawlScheduler recrawlScheduler;
	private DocumentSimilarityChecker similarityChecker;
	private String pagesFolder, crawlDbPath, extractedPath;

	private int crawlerNum;

	private float similarityThreshold;

	/**
	 * Constructs a new instance.
	 * 
	 * @param seedUrls
	 *            The seedUrls for this instance.
	 */
	public CrawlerRunnable(int crawlerNum,
			XMLConfiguration crawlerConfiguration, Host[] hosts,
			FetchScheduler scheduler, RecrawlScheduler recrawlScheduler,
			DocumentSimilarityChecker similarityChecker) {
		this.hosts = hosts;
		this.linkQueues = new LinkQueue[hosts.length];
		this.urlTable = new UrlTable(100000);
		this.pageTable = new PageTable(100000);
		this.fetchScheduler = scheduler;
		this.recrawlScheduler = recrawlScheduler;

		this.similarityChecker = similarityChecker;
		this.similarityThreshold = crawlerConfiguration
				.getFloat("similarityThreshold"); //$NON-NLS-1$
		this.crawlerNum = crawlerNum;
		this.crawlerName = crawlerConfiguration.getString("crawlerName"); //$NON-NLS-1$

		this.connectionTimeout = (int) (crawlerConfiguration
				.getFloat("timeSettings.connectionTimeout")); //$NON-NLS-1$
		this.readTimeout = (int) (crawlerConfiguration
				.getFloat("timeSettings.readTimeout")); //$NON-NLS-1$
		HttpParams params = new BasicHttpParams();
		HttpConnectionParams.setConnectionTimeout(params,
				this.connectionTimeout * 1000);
		HttpConnectionParams.setSoTimeout(params, this.readTimeout * 1000);
		this.httpClient = new DefaultHttpClient(params);

		this.extractedPath = crawlerConfiguration
				.getString("paths.extractedDocuments"); //$NON-NLS-1$
		new File(this.extractedPath).mkdirs();
		this.crawlDbPath = crawlerConfiguration
				.getString("paths.crawlDatabase"); //$NON-NLS-1$
		this.pagesFolder = new File(this.crawlDbPath, "pages").toString(); //$NON-NLS-1$

		/*
		 * Statistics
		 */
		this.numWordsHistory = new int[50];
		this.fetchDurationHistory = new long[50];
		this.lastHistoryPos = 0;
		this.numTotalPages = this.numTotalWords = 0;
		this.averageCrawlDelay = 0;

		logger.trace(""); //$NON-NLS-1$
		for (int i = 0; i < hosts.length; i++) {
			int url = this.urlTable.add(hosts[i].getUrl().toString());
			this.linkQueues[i] = new LinkQueue(hosts[i],
					System.currentTimeMillis());
			this.linkQueues[i].add(url);
			logger.trace("", new Object[] { //$NON-NLS-1$
					hosts[i].getUrl(), url });
			this.averageCrawlDelay += hosts[i].getCrawlDelay();
		}
		this.averageCrawlDelay /= hosts.length;
		logger.trace(""); //$NON-NLS-1$
	}

	@SuppressWarnings("unused")
	private CrawlerRunnable() {
		super();
	}

	/**
	 * Updates moving average of performance values for this instance.
	 * 
	 * @param newNumWords
	 *            new number of words for this instant
	 * @param newFetchDuration
	 *            new fetch duration for this instant
	 */
	void calculateMovingAverage(int newNumWords, long newFetchDuration) {
		this.movingWordAverage = this.movingWordAverage
				- (this.numWordsHistory[this.lastHistoryPos] - newNumWords)
				/ 50.0F;
		this.fetchDurationAverage = this.fetchDurationAverage
				- (this.fetchDurationHistory[this.lastHistoryPos] - newFetchDuration)
				/ 50L;

		this.wordsPerSecond = this.movingWordAverage * 1000
				/ (this.fetchDurationAverage + this.averageCrawlDelay);

		this.numWordsHistory[this.lastHistoryPos] = newNumWords;
		this.fetchDurationHistory[this.lastHistoryPos] = newFetchDuration;
		this.lastHistoryPos++;
		if (this.lastHistoryPos == 50)
			this.lastHistoryPos = 0;
	}

	/**
	 * Deserializes a page from the disk, checks if the page has changed and
	 * updates page status accordingly.
	 * 
	 * @param lastFetch
	 *            the time at which this page was fetched the last
	 * @param pageUrlEntry
	 *            page number that is associated with this {@link Page}
	 * @param checksum
	 *            checksum of the page's text contents
	 * @param htmlDoc
	 *            HTML document that was obtained at last fetch
	 * @return
	 * @throws BoilerpipeProcessingException
	 * @throws IOException
	 *             when there are deserialization problems
	 */
	Page getAndUpdatePage(long lastFetch, int pageUrlEntry, String checksum,
			String htmlDoc) throws BoilerpipeProcessingException, IOException {
		File pageFile = new File(this.pagesFolder, this.crawlerNum + "pf" //$NON-NLS-1$
				+ String.valueOf(pageUrlEntry));

		Page page = FileUtils.deserializeObject(pageFile, Page.class);

		if (!page.getChecksum().equals(checksum)) {
			/*
			 * Checksum has changed, possible update
			 */
			page.setChecksum(checksum);
			String allText = Parser.getInstance().extractText(htmlDoc);
			if (!this.similarityChecker.isSimilar(page.getAllTextContent(),
					allText, this.similarityThreshold)) {
				page.setAllTextContent(allText);
				page.updateFetchTimes(lastFetch, true);
			} else {
				page.updateFetchTimes(lastFetch, false);
			}
		} else {
			page.updateFetchTimes(lastFetch, false);
		}

		return page;
	}

	/**
	 * @return the associatedThread
	 */
	public Thread getAssociatedThread() {
		return this.associatedThread;
	}

	/**
	 * @return the crawlDbPath
	 */
	public final String getCrawlDbPath() {
		return this.crawlDbPath;
	}

	/**
	 * @return the crawlerNum
	 */
	public final int getCrawlerNum() {
		return this.crawlerNum;
	}

	/**
	 * @return the fetchDurationAverage
	 */
	public float getFetchDurationAverage() {
		return this.fetchDurationAverage;
	}

	/**
	 * @return the linkQueues
	 */
	public final LinkQueue[] getLinkQueues() {
		return this.linkQueues;
	}

	/**
	 * @return the movingWordAverage
	 */
	public float getMovingWordAverage() {
		return this.movingWordAverage;
	}

	/**
	 * @return the numTotalPages
	 */
	public long getNumTotalPages() {
		return this.numTotalPages;
	}

	/**
	 * @return the numTotalWords
	 */
	public long getNumTotalWords() {
		return this.numTotalWords;
	}

	/**
	 * @return the pagesZipFile
	 */
	public final String getPagesFolder() {
		return this.pagesFolder;
	}

	/**
	 * @return the urlTable
	 */
	public final UrlTable getUrlTable() {
		return this.urlTable;
	}

	/**
	 * @return the wordsPerSecond
	 */
	public float getWordsPerSecond() {
		return this.wordsPerSecond;
	}

	String readContent(String url) throws IOException {
		HttpGet httpget = new HttpGet(url.toString());
		httpget.setHeader("User-Agent", this.crawlerName + "/0.1"); //$NON-NLS-1$ //$NON-NLS-2$
		HttpResponse response = this.httpClient.execute(httpget);
		if (response == null) {
			throw new IOException();
		}
		HttpEntity entity = response.getEntity();
		if (entity == null)
			throw new IOException();

		Charset charset = ContentType.get(entity).getCharset();
		if (charset == null) {
			charset = Charset.forName("utf-8"); //$NON-NLS-1$
		}

		StringBuilder sb = new StringBuilder(250000);
		InputStream is = entity.getContent();
		BufferedReader br = new BufferedReader(new InputStreamReader(is,
				charset));
		String input;
		while ((input = br.readLine()) != null) {
			sb.append(input + System.getProperty("line.separator")); //$NON-NLS-1$
		}
		br.close();
		is.close();

		String result = sb.toString();
		if (result == null || result.isEmpty())
			throw new IOException();

		return result;
	}

	/**
	 * This method reinitializes transient members of the object. Useful and
	 * necessary for waking up after deserialization.
	 */
	public void reinitializeTransients() {
		HttpParams params = new BasicHttpParams();
		HttpConnectionParams.setConnectionTimeout(params,
				this.connectionTimeout * 1000);
		HttpConnectionParams.setSoTimeout(params, this.readTimeout * 1000);
		this.httpClient = new DefaultHttpClient(params);
		new File(this.extractedPath).mkdirs();
	}

	@Override
	/**
	 * Runnable method. Starts crawling and extracting.
	 */
	public void run() {
		while (true) {
			/*
			 * Check if there are still links to fetch
			 */
			boolean found = false;
			for (int i = 0; i < this.linkQueues.length; i++) {
				if (this.linkQueues[i].size() > 0) {
					found = true;
				}
			}

			if (!found) {
				logger.info("No links left for thread {}, terminating thread...", this.crawlerNum+1);
				break;
			}
			/*
			 * Find the earliest URL that we must fetch (or must have fetched)
			 */
			int currentQueue = this.fetchScheduler
					.getNextQueue(this.linkQueues);

			/*
			 * Sleep until we are able to fetch the next URL
			 */
			try {
				Thread.sleep(this.fetchScheduler.getSleepAmountUntilNextFetch(
						this.linkQueues[currentQueue],
						System.currentTimeMillis()));
			} catch (InterruptedException e) {
				logger.warn(Messages
						.getString("CrawlerRunnable.ThreadWasAwoken")); //$NON-NLS-1$
				logger.debug(ExceptionUtils.getStackTrace(e));
			}

			int pageUrlEntry = this.linkQueues[currentQueue].poll();
			String pageUrl = this.urlTable.getUrl(pageUrlEntry);

			/*
			 * Read the content as a String
			 */
			String htmlDoc = null;
			long fetchStart = System.currentTimeMillis();
			try {
				htmlDoc = readContent(pageUrl);
			} catch (Exception e) {
				logger.warn(
						Messages.getString("CrawlerRunnable.ErrorWhileFetching"), pageUrl); //$NON-NLS-1$
				logger.debug(ExceptionUtils.getStackTrace(e));
				continue;
			}

			long fetchEnd = System.currentTimeMillis();
			this.linkQueues[currentQueue].setLastFetchTime(fetchEnd);
			this.linkQueues[currentQueue].setLastFetchDuration(fetchEnd
					- fetchStart);

			/*
			 * Parse it
			 */
			String extractedText = null;
			try {
				extractedText = Parser.getInstance().parseArticle(htmlDoc);
			} catch (BoilerpipeProcessingException e) {
				logger.warn(
						Messages.getString("CrawlerRunnable.ErrorWhileParsing"), pageUrl); //$NON-NLS-1$
				logger.debug(ExceptionUtils.getStackTrace(e));
				continue;
			}

			if (extractedText == null) {
				logger.warn(
						Messages.getString("CrawlerRunnable.ErrorWhileParsing"), pageUrl); //$NON-NLS-1$
				continue;
			}

			/*
			 * Save the extracted document as a file. Filename is a hash of
			 * parsed contents
			 */
			if (extractedText.length() < 20) {
				logger.info(Messages
						.getString("CrawlerRunnable.NoMeaningfulTextOutput"), //$NON-NLS-1$
						pageUrl);
			} else {
				String filename = Parser.getInstance().hash256(extractedText);
				File file = new File(this.extractedPath, filename);
				try {
					BufferedWriter writer = new BufferedWriter(
							new OutputStreamWriter(new FileOutputStream(file),
									Charset.forName("utf-8"))); //$NON-NLS-1$
					// FileUtils.writeLines(file, extractedText,
					// Charset.forName("utf-8"));
					String[] lines = extractedText.split("[\\r\\n]+"); //$NON-NLS-1$
					for (String line : lines) {
						writer.write(line
								+ System.getProperty("line.separator")); //$NON-NLS-1$
					}
					writer.close();
				} catch (IOException e) {
					logger.error(Messages
							.getString("CrawlerRunnable.ErrorWhileParsing"), //$NON-NLS-1$
							pageUrl);
					logger.debug(ExceptionUtils.getStackTrace(e));
					System.exit(1);
				}
			}

			/*
			 * Count words for statistics
			 */
			int newNumWords = extractedText.split("([.,!?:;'\"-]|\\s)+").length; //$NON-NLS-1$
			this.numTotalWords += newNumWords;
			calculateMovingAverage(newNumWords, fetchEnd - fetchStart);

			/*
			 * Extract links
			 */
			ArrayList<String> links = null;
			try {
				links = Parser.extractLinks(new URL(pageUrl), htmlDoc);
			} catch (MalformedURLException e) {
				logger.warn(
						Messages.getString("CrawlerRunnable.ErrorWhileExtractingLinks"), //$NON-NLS-1$
						pageUrl);
				logger.debug(ExceptionUtils.getStackTrace(e));
				continue;
			}

			removeDisallowedAndOutgoing(links, this.hosts[currentQueue]);

			if (links.size() == 0) {
				logger.warn(
						Messages.getString("CrawlerRunnable.NoLinksWereExtracted"), pageUrl); //$NON-NLS-1$
			}

			// TODO: Check if all linkqueues are empty and links are empty, then
			// something is wrong, terminate

			/*
			 * Add the links to queue for crawling and get url entries for them
			 */
			ArrayList<Integer> newEntries = new ArrayList<Integer>();
			for (int i = 0; i < links.size(); i++) {
				String link = links.get(i);
				int url;
				if ((url = this.urlTable.add(link.toString())) != -1) {
					newEntries.add(url);
				}
			}

			this.linkQueues[currentQueue].addAll(newEntries);

			/*
			 * Everything is fine so far. Update the page if exists, else create
			 * a new one
			 */
			String checksum = Parser.getInstance().hash256(htmlDoc);
			Page page = null;
			if (this.pageTable.contains(pageUrlEntry)) {
				try {
					page = getAndUpdatePage(fetchEnd, pageUrlEntry, checksum,
							htmlDoc);
				} catch (BoilerpipeProcessingException e) {
					logger.warn(
							Messages.getString("CrawlerRunnable.ErrorWhileReadingLocalCacheButTextWasExtracted"), //$NON-NLS-1$
							pageUrl);
					logger.debug(ExceptionUtils.getStackTrace(e));
					continue;
				} catch (IOException e) {
					logger.warn(
							Messages.getString("CrawlerRunnable.ErrorWhileReadingLocalCacheButTextWasExtracted"), //$NON-NLS-1$
							pageUrl);
					logger.debug(ExceptionUtils.getStackTrace(e));
					continue;
				}
			} else {
				String allText = null;
				try {
					allText = Parser.getInstance().extractText(htmlDoc);
				} catch (BoilerpipeProcessingException e) {
					logger.warn(
							Messages.getString("CrawlerRunnable.ErrorWhileParsingButTextWasExtracted"), //$NON-NLS-1$
							pageUrl);
					logger.debug(ExceptionUtils.getStackTrace(e));
					continue;
				}
				page = new Page(pageUrlEntry, checksum, allText, fetchEnd);

			}

			/*
			 * Write the page to local cache
			 */
			File pageFile = new File(this.pagesFolder, this.crawlerNum + "pf" //$NON-NLS-1$
					+ pageUrlEntry);

			try {
				FileUtils.serializeObject(pageFile, page);
			} catch (FileNotFoundException e) {
				logger.warn(
						Messages.getString("CrawlerRunnable.ErrorWhileWritingPageToDiskButTextWasExtracted"), //$NON-NLS-1$
						pageUrl);
			}

			/*
			 * Add the URL to the queue for recrawling
			 */
			this.linkQueues[currentQueue].add(pageUrlEntry,
					this.recrawlScheduler.getNextRecrawlTime(page));

			this.numTotalPages++;

			if (logger.isTraceEnabled()) {
				for (int i = 0; i < this.linkQueues.length; i++) {
					logger.trace(this.linkQueues[i].getStats(this.urlTable));
				}
			}

			
		}
	}

	/**
	 * @param associatedThread
	 *            the associatedThread to set
	 */
	public void setAssociatedThread(Thread associatedThread) {
		this.associatedThread = associatedThread;
	}

	/**
	 * @param httpClient
	 *            the httpClient to set
	 */
	public final void setHttpClient(HttpClient httpClient) {
		this.httpClient = httpClient;
	}
}
