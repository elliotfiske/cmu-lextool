/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

/**
 * A scheduler interface for scheduling fetches in threads with multiple
 * {@link LinkQueue} objects, which means a thread has more than one seed host.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public interface FetchScheduler {
	/**
	 * A method that must return the queue number that will be polled next according to implemented scheduling scheme.
	 * @param queues an array of all {@link LinkQueue} objects that are associated with a {@link CrawlerRunnable} instance.
	 * @return the queue number that will be polled next
	 */
	public int getNextQueue(LinkQueue[] queues);

	public long getNextFetchTime(LinkQueue queue);

	public long getSleepAmountUntilNextFetch(LinkQueue queue, long currentTime);
}
