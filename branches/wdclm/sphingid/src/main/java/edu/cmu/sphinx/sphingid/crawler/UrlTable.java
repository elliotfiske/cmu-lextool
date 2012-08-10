/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;

/**
 * A table that holds String objects for URLs.
 * <p>
 * Each URL is represented by an integer in the crawler to save memory. This
 * data structure allows tracking the relation between URLs and numbers.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class UrlTable implements Serializable {
	private static final long serialVersionUID = -6450769551453787800L;
	private ArrayList<Long> hashes;
	private ArrayList<String> urlList;
	private int pos;

	public UrlTable(int initialCapacity) {
		this.urlList = new ArrayList<String>(initialCapacity);
		this.hashes = new ArrayList<Long>(initialCapacity);
		this.pos = -1;
	}

	public int add(String url) {
		Parser parser = Parser.getInstance();
		long hash = parser.hash64(url);
		int hashPos = Collections.binarySearch(this.hashes, hash);
		if (this.hashes.size() == 0) {
			this.pos++;
			this.hashes.add(hash);
			this.urlList.add(url);

			return this.pos;
		} else if (hashPos < 0) {
			this.pos++;
			hashPos = hashPos * -1 - 1;
			if (hashPos == size()) {
				this.hashes.add(hash);
			} else {
				this.hashes.add(hashPos, hash);
			}
			this.urlList.add(this.pos, url);
			return this.pos;
		} else
			return -1;
	}

	public boolean containsUrl(String url) {
		Parser parser = Parser.getInstance();
		long hash = parser.hash64(url);
		int pos = Collections.binarySearch(this.hashes, hash);
		return pos >= 0;
	}

	public String getUrl(int pos) {
		if (pos > this.pos) {
			throw new ArrayIndexOutOfBoundsException();
		}
		return this.urlList.get(pos);
	}

	public int size() {
		return this.urlList.size();
	}
}
