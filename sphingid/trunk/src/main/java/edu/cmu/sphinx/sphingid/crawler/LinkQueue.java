package edu.cmu.sphinx.sphingid.crawler;

import java.util.ArrayList;
import java.util.NoSuchElementException;

/**
 * 
 * A queue implementation that holds links that will be fetched in it.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class LinkQueue {
	private LinkQueueEntry first;
	private Host host;
	private int size;

	/*
	 * This variable holds a pointer to the latest time entry. This is necessary
	 * since we need to add entries to the queue, but not after crawl delay. If
	 * we use a simple scheme of adding links per crawlDelay * i +
	 * System.currentTimeMillis(), then the pages that were added first will be
	 * discriminated. We can keep on adding links to crawl to the end of
	 * "crawl queue" by using a pointer like this. This is also necessary for
	 * supporting recrawling.
	 */
	private long queueTimePointer;

	private long lastFetchTime;
	private long lastFetchDuration;

	public LinkQueue(Host associatedHost, long currentTime) {
		this.host = associatedHost;
		this.first = null;
		this.size = 0;
		this.queueTimePointer = currentTime;
	}
	
	private LinkQueue() {
		super();
	}

	/**
	 * Adds a link to the queue for crawling.
	 * <p>
	 * If multiple links will be added, use {@link addAll} instead.
	 * 
	 * @param link
	 * @param nextFetchTime
	 */
	public void add(int link) {
		this.queueTimePointer += getCrawlDelay();
		LinkQueueEntry newEntry = new LinkQueueEntry(link,
				this.queueTimePointer);

		if (this.size == 0) {
			this.first = newEntry;
			this.size++;
		} else {
			LinkQueueEntry pointer = this.first;

			if (pointer.compareTo(newEntry) == 1) {
				newEntry.setNext(pointer);
				this.first = newEntry;
				this.size++;
				return;
			}

			while (pointer.getNext() != null
					&& pointer.getNext().compareTo(newEntry) < 1) {
				pointer = pointer.getNext();
			}

			if (pointer.getNext() != null) {
				newEntry.setNext(pointer.getNext());
			}

			pointer.setNext(newEntry);
			this.size++;
		}
	}

	public void add(int link, long suggestedFetchTime) {
		LinkQueueEntry newEntry = new LinkQueueEntry(link, suggestedFetchTime);

		if (this.size == 0) {
			this.first = newEntry;
			this.size++;
		} else {
			LinkQueueEntry pointer = this.first;

			if (pointer.compareTo(newEntry) == 1) {
				newEntry.setNext(pointer);
				this.first = newEntry;
				this.size++;
				return;
			}

			while (pointer.getNext() != null
					&& pointer.getNext().compareTo(newEntry) < 1) {
				pointer = pointer.getNext();
			}

			if (pointer.getNext() != null) {
				newEntry.setNext(pointer.getNext());
			}

			pointer.setNext(newEntry);
			this.size++;
		}
	}

	/**
	 * Add all of the links in the list to the queue.
	 * <p>
	 * This is the preferred method for adding multiple entries. This method has
	 * a lower time complexity than calling {@link add} for non-empty lists.
	 * 
	 * @param link
	 * @param nextFetchTime
	 */
	public void addAll(ArrayList<Integer> links) {
		if (links.size() == 0)
			return;

		if (this.size == 0) {
			this.queueTimePointer += getCrawlDelay();
			LinkQueueEntry newEntry = new LinkQueueEntry(links.get(0),
					this.queueTimePointer);
			this.first = newEntry;
			this.size++;
			LinkQueueEntry pointer = this.first;
			for (int i = 1; i < links.size(); i++) {
				this.queueTimePointer += getCrawlDelay();
				newEntry = new LinkQueueEntry(links.get(i),
						this.queueTimePointer);
				pointer.setNext(newEntry);
				pointer = newEntry;
				this.size++;
			}
		} else {
			LinkQueueEntry pointer = this.first;
			for (int linkPos = 0; linkPos < links.size(); linkPos++) {
				this.queueTimePointer += getCrawlDelay();
				LinkQueueEntry newEntry = new LinkQueueEntry(
						links.get(linkPos), this.queueTimePointer);

				if (pointer == this.first && pointer.compareTo(newEntry) == 1) {
					newEntry.setNext(pointer);
					this.first = newEntry;
					pointer = this.first;
					this.size++;
				} else {
					while (pointer.getNext() != null
							&& pointer.getNext().compareTo(newEntry) < 1) {
						pointer = pointer.getNext();
					}

					if (pointer.getNext() != null) {
						newEntry.setNext(pointer.getNext());
					}

					pointer.setNext(newEntry);
					pointer = newEntry;
					this.size++;
				}
			}
		}
	}

	/**
	 * Returns crawl delay of the associated host in seconds.
	 * 
	 * @return crawl delay in seconds
	 */
	public long getCrawlDelay() {
		return this.host.getCrawlDelay();
	}

	/**
	 * @return the lastFetchDuration
	 */
	public long getLastFetchDuration() {
		return this.lastFetchDuration;
	}

	public long getLastFetchTime() {
		return this.lastFetchTime;
	}

	public String getStats(UrlTable table) {
		StringBuilder sb = new StringBuilder();
		LinkQueueEntry pointer = this.first;
		while (pointer != null) {
			sb.append("U: "); //$NON-NLS-1$
			sb.append(pointer.getUrlEntry());
			sb.append(", FT: "); //$NON-NLS-1$
			sb.append(pointer.getSuggestedFetchTime());
			sb.append(", URL: "); //$NON-NLS-1$
			sb.append(table.getUrl(pointer.getUrlEntry()));
			sb.append(System.getProperty("line.separator")); //$NON-NLS-1$
			pointer = pointer.getNext();
		}
		return sb.toString();
	}

	public boolean isEmpty() {
		return (this.size == 0);
	}

	public int peek() {
		if (this.size == 0) {
			throw new NoSuchElementException();
		}

		LinkQueueEntry result = this.first;
		return result.getUrlEntry();
	}

	public long peekFetchTime() {
		if (this.size == 0) {
			throw new NoSuchElementException();
		}

		LinkQueueEntry result = this.first;
		return result.getSuggestedFetchTime();
	}

	public int poll() {
		if (this.size == 0) {
			throw new NoSuchElementException();
		}
		LinkQueueEntry result = this.first;

		if (this.size == 1) {
			this.first = null;
		} else {
			this.first = this.first.getNext();
		}
		this.size--;
		return result.getUrlEntry();
	}

	/**
	 * @param lastFetchDuration
	 *            the lastFetchDuration to set
	 */
	public void setLastFetchDuration(long lastFetchDuration) {
		this.lastFetchDuration = lastFetchDuration;
	}

	public void setLastFetchTime(long lastFetchTime) {
		this.lastFetchTime = lastFetchTime;
	}

	public int size() {
		return this.size;
	}

}
