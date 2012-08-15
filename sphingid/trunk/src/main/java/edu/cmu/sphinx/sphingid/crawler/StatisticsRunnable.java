/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

/**
 * A {@link Runnable} class that periodically logs performance statuses of each
 * {@link CrawlerRunnable} instance that was given to it.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class StatisticsRunnable implements Runnable {
	private CrawlerRunnable[] crawlerRunnables;
	private long displayStatisticsInterval;
	private static final Logger logger = LoggerFactory
			.getLogger(StatisticsRunnable.class);

	public StatisticsRunnable(CrawlerRunnable[] crawlerRunnables,
			long displayStatisticsInterval) {
		this.crawlerRunnables = crawlerRunnables;
		this.displayStatisticsInterval = displayStatisticsInterval;
	}

	@Override
	public void run() {
		long startTime = System.currentTimeMillis();
		boolean isThreadsStillRunning;
		do {
			try {
				Thread.sleep(this.displayStatisticsInterval);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			isThreadsStillRunning = false;
			float totalWPS = 0, totalAvgWords = 0, totalAvgDurations = 0;
			long totalWords = 0, totalPages = 0;
			for (int i = 0; i < this.crawlerRunnables.length; i++) {
				if (this.crawlerRunnables[i].getAssociatedThread().isAlive()) {
					isThreadsStillRunning = true;
					logger.debug(Messages.getString("SStatisticsRunnable.AverageWPS"), //$NON-NLS-1$
							i, this.crawlerRunnables[i].getWordsPerSecond());
					logger.debug(
							Messages.getString("StatisticsRunnable.AverageWPF"), //$NON-NLS-1$
							this.crawlerRunnables[i].getMovingWordAverage(),
							this.crawlerRunnables[i].getFetchDurationAverage());
					totalWPS += this.crawlerRunnables[i].getWordsPerSecond();
					totalAvgWords += this.crawlerRunnables[i]
							.getMovingWordAverage();
					totalAvgDurations += this.crawlerRunnables[i]
							.getFetchDurationAverage();
				}
				logger.debug(
						Messages.getString("StatisticsRunnable.TotalWords"), //$NON-NLS-1$
						new Object[] { new Integer(i),
								this.crawlerRunnables[i].getNumTotalWords(),
								new Integer(i),
								this.crawlerRunnables[i].getNumTotalPages() });

				totalWords += this.crawlerRunnables[i].getNumTotalWords();
				totalPages += this.crawlerRunnables[i].getNumTotalPages();
			}

			logger.info(Messages.getString("StatisticsRunnable.TotalWPS"), totalWPS); //$NON-NLS-1$
			logger.info(Messages.getString("StatisticsRunnable.TotalMovingAverages"), //$NON-NLS-1$
					totalAvgWords, totalAvgDurations);
			logger.info(Messages.getString("StatisticsRunnable.GrandTotalWords"), //$NON-NLS-1$
					totalWords, totalPages);
			long secondsElapsed = (System.currentTimeMillis() - startTime) / 1000;
			float hoursElapsed = secondsElapsed / 3600.0F;
			logger.info(Messages.getString("StatisticsRunnable.GrandTotalWPS"), //$NON-NLS-1$
					new Object[] { secondsElapsed, hoursElapsed,
							totalWords / hoursElapsed,
							totalPages / hoursElapsed });

		} while (isThreadsStillRunning);
	}
}
