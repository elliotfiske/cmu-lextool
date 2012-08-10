/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.Serializable;
import java.util.ArrayList;

/**
 * 
 * A list that contains number entries of fetched and parsed pages.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class PageTable implements Serializable {
	private static final long serialVersionUID = 3957478693525588545L;
	private ArrayList<Boolean> table;

	public PageTable() {
		this.table = new ArrayList<Boolean>();
	}

	public void add(int urlEntry) {
		this.table.add(urlEntry, true);
	}

	public Boolean contains(int urlEntry) {
		if (urlEntry < this.table.size() && this.table.get(urlEntry) != null) {
			return true;
		}
		return false;
	}

}
