package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;

/**
 * 
 * A class for entries of {@link LinkQueue}.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class LinkQueueEntry implements Comparable<LinkQueueEntry> {
	private int urlEntry;
	private long suggestedFetchTime;
	private LinkQueueEntry next;

	/**
	 * @param urlEntry
	 * @param suggestedFetchTime
	 */
	public LinkQueueEntry(int urlEntry, long suggestedFetchTime) {
		this.urlEntry = urlEntry;
		this.suggestedFetchTime = suggestedFetchTime;
	}
	
	private LinkQueueEntry() {
		super();
	}

	@Override
	public int compareTo(LinkQueueEntry linkQueueEntry) {
		if (this.suggestedFetchTime < linkQueueEntry.getSuggestedFetchTime())
			return -1;
		else if (this.suggestedFetchTime == linkQueueEntry
				.getSuggestedFetchTime())
			return 0;
		else
			return 1;
	}

	/**
	 * @return the next
	 */
	public LinkQueueEntry getNext() {
		return this.next;
	}

	/**
	 * @return the suggestedFetchTime
	 */
	public long getSuggestedFetchTime() {
		return this.suggestedFetchTime;
	}

	/**
	 * @return the urlEntry
	 */
	public int getUrlEntry() {
		return this.urlEntry;
	}

	/**
	 * @param next
	 *            the next to set
	 */
	public void setNext(LinkQueueEntry next) {
		this.next = next;
	}

	/**
	 * @param suggestedFetchTime
	 *            the suggestedFetchTime to set
	 */
	public void setSuggestedFetchTime(long suggestedFetchTime) {
		this.suggestedFetchTime = suggestedFetchTime;
	}

}
