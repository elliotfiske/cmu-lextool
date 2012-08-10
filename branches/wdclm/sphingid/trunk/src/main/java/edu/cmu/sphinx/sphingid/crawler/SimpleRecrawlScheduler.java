/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;

/**
 * A simple recrawl scheduler. It works by dividing recrawl interval of a page
 * by a user-set amount if the page is updated at a fetch. If the page was not
 * updated, it multiplies instead.
 * <p>
 * There is a minimum recrawl interval value that prevents dynamic pages from
 * being fetched too frequently.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class SimpleRecrawlScheduler implements RecrawlScheduler, Serializable {
	private static final long serialVersionUID = -1425722222101992929L;
	private long defaultRecrawlInterval;
	private long minimumRecrawlInterval;
	private float recrawlTimeMultiplier;

	/**
	 * @param defaultRecrawlInterval
	 * @param minimumRecrawlInterval
	 * @param recrawlTimeMultiplier
	 */
	public SimpleRecrawlScheduler(long defaultRecrawlInterval,
			long minimumRecrawlInterval, float recrawlTimeMultiplier) {
		super();
		this.defaultRecrawlInterval = defaultRecrawlInterval;
		this.minimumRecrawlInterval = minimumRecrawlInterval;
		this.recrawlTimeMultiplier = recrawlTimeMultiplier;
	}

	@Override
	public long getNextRecrawlTime(Page page) {
		if (page.isFetchedOnlyOnce()) {
			return page.getLastFetchTime() + this.defaultRecrawlInterval;
		}

		int historyLength = page.getFetchTimes().length - 1;
		long lastRecrawlInterval = page.getFetchTimes()[historyLength]
				- page.getFetchTimes()[historyLength - 1];
		long recrawlInterval;

		if (page.isUpdatedLastFetch()) {
			recrawlInterval = (long) (lastRecrawlInterval / this.recrawlTimeMultiplier);
			if (recrawlInterval < this.minimumRecrawlInterval) {
				recrawlInterval = this.minimumRecrawlInterval;
			}
		} else
			recrawlInterval = (long) (lastRecrawlInterval * this.recrawlTimeMultiplier);

		return page.getLastFetchTime() + recrawlInterval;
	}
}
