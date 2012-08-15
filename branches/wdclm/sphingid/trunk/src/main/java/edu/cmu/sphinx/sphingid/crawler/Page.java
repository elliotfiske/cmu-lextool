package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;

/**
 * A class for representing web pages.
 * 
 * @author Emre Çelikten
 * 
 */
public class Page  {
	private String allTextContent;
	private String checksum;
	private long[] fetchTimes;
	private boolean[] fetchUpdateStatuses;
	private int url;

	/**
	 * @param url
	 * @param checksum
	 * @param outgoingLinks
	 * @param lastFetchTime
	 * @param recrawlInterval
	 */
	public Page(int url, String checksum, String allTextContent,
			long lastFetchTime) {
		this.url = url;
		this.checksum = checksum;
		this.allTextContent = allTextContent;
		this.fetchTimes = new long[20];
		this.fetchUpdateStatuses = new boolean[20];
		this.fetchTimes[19] = lastFetchTime;
		this.fetchUpdateStatuses[19] = true;
	}
	
	private Page() {
		super();
	}

	/**
	 * @return the allTextContent
	 */
	public String getAllTextContent() {
		return this.allTextContent;
	}

	/**
	 * @return the checksum
	 */
	public String getChecksum() {
		return this.checksum;
	}

	/**
	 * @return the fetchTimes
	 */
	public long[] getFetchTimes() {
		return this.fetchTimes;
	}

	/**
	 * @return the isUpdatedAtFetches
	 */
	public boolean[] getFetchUpdateStatuses() {
		return this.fetchUpdateStatuses;
	}

	/**
	 * @return the lastFetchTime
	 */
	public long getLastFetchTime() {
		return this.fetchTimes[this.fetchTimes.length - 1];
	}

	/**
	 * @return the url
	 */
	public int getUrl() {
		return this.url;
	}

	public boolean isFetchedOnlyOnce() {
		return getFetchTimes()[this.fetchUpdateStatuses.length - 2] == 0;
	}

	public boolean isUpdatedLastFetch() {
		return this.fetchUpdateStatuses[this.fetchUpdateStatuses.length - 1];
	}

	/**
	 * @param allTextContent
	 *            the allTextContent to set
	 */
	public void setAllTextContent(String allTextContent) {
		this.allTextContent = allTextContent;
	}

	/**
	 * @param checksum
	 *            the checksum to set
	 */
	public void setChecksum(String checksum) {
		this.checksum = checksum;
	}

	/**
	 * Remove and shift
	 * 
	 * @param lastFetchTime
	 */
	public void updateFetchTimes(long fetchTime, boolean isUpdated) {
		for (int i = 0; i < this.fetchTimes.length - 1; i++) {
			this.fetchTimes[i] = this.fetchTimes[i + 1];
			this.fetchUpdateStatuses[i] = this.fetchUpdateStatuses[i + 1];
		}

		this.fetchTimes[this.fetchTimes.length - 1] = fetchTime;
		this.fetchUpdateStatuses[this.fetchTimes.length - 1] = isUpdated;
	}

}
