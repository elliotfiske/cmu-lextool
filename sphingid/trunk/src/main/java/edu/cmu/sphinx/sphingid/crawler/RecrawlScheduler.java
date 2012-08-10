package edu.cmu.sphinx.sphingid.crawler;

/**
 * Interface for recrawl scheduling algorithms.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public interface RecrawlScheduler {
	public long getNextRecrawlTime(Page page);
}
