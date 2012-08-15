/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.File;
import java.io.IOException;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import edu.cmu.sphinx.sphingid.commons.FileUtils;

/**
 * A {@link Runnable} class that periodically saves status for each running
 * {@link CrawlerRunnable} using serialization.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class CrawlStateWriterRunnable implements Runnable {
	private CrawlerRunnable[] crawlerRunnables;
	private long crawlStateWriteInterval;
	private static final Logger logger = LoggerFactory
			.getLogger(CrawlStateWriterRunnable.class);

	public CrawlStateWriterRunnable(CrawlerRunnable[] crawlerRunnables,
			long crawlStateWriteInterval) {
		this.crawlerRunnables = crawlerRunnables;
		this.crawlStateWriteInterval = crawlStateWriteInterval;
	}

	public void run() {
		boolean isThreadsStillRunning;
		do {
			try {
				Thread.sleep(this.crawlStateWriteInterval);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			isThreadsStillRunning = false;

			for (int i = 0; i < this.crawlerRunnables.length; i++) {
				if (this.crawlerRunnables[i].getAssociatedThread().isAlive()) {
					isThreadsStillRunning = true;

					try {
						writeCrawlerStateToDisk(this.crawlerRunnables[i]);
					} catch (IOException e) {
						// TODO Auto-generated catch block
						e.printStackTrace();
					}
				}
			}
			logger.info(Messages
					.getString("CrawlStateWriterRunnable.CrawlerStatesWasWritten")); //$NON-NLS-1$
		} while (isThreadsStillRunning);
	}

	static void writeCrawlerStateToDisk(CrawlerRunnable crawlerRunnable)
			throws IOException {
		File state = new File(crawlerRunnable.getCrawlDbPath(),
				"crawler" + crawlerRunnable.getCrawlerNum() //$NON-NLS-1$
						+ ".ser");
		FileUtils.serializeObject(state, crawlerRunnable); //$NON-NLS-1$
	}
}
