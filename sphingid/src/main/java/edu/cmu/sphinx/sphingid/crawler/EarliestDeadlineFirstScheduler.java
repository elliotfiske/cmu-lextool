/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;

/**
 * A basic {@link FetchScheduler} implementation that tries to prioritize pages
 * that will finish the earliest. It will also prefer pages that will finish
 * later if the earliest one is not ready to be fetched (i.e. scheduled for
 * future or waiting under crawl delay) for not staying idle.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class EarliestDeadlineFirstScheduler implements FetchScheduler,
		Serializable {
	private static final long serialVersionUID = 5711499624459871445L;

	@Override
	public long getNextFetchTime(LinkQueue queue) {
		long queueNextFetchTime = queue.getLastFetchTime()
				+ queue.getCrawlDelay();
		long pageNextFetchTime = queue.peekFetchTime();
		return Math.max(queueNextFetchTime, pageNextFetchTime)
				+ queue.getLastFetchDuration();
	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see
	 * edu.cmu.sphinx.sphingid.crawler.FetchScheduler#getNextFetchTime(edu.cmu
	 * .sphinx.sphingid.crawler.Host)
	 */
	@Override
	public int getNextQueue(LinkQueue[] queues) {
		long earliestDeadline = Long.MAX_VALUE;
		int earliestQueue = -1;
		int earliestReadyQueue = -1;
		for (int i = 0; i < queues.length; i++) {
			if (getNextFetchTime(queues[i]) < earliestDeadline) {
				earliestDeadline = getNextFetchTime(queues[i]);
				earliestQueue = i;

				if (queues[i].peekFetchTime() <= System.currentTimeMillis()) {
					earliestReadyQueue = earliestQueue;
				}
			}
		}

		if (earliestReadyQueue != -1)
			return earliestReadyQueue;

		return earliestQueue;
	}

	@Override
	public long getSleepAmountUntilNextFetch(LinkQueue queue, long currentTime) {
		long queueNextFetchTime = queue.getLastFetchTime()
				+ queue.getCrawlDelay();
		long pageNextFetchTime = queue.peekFetchTime();
		return Math.max(Math.max(queueNextFetchTime, pageNextFetchTime)
				- currentTime, 0);
	}
}
