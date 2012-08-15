/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.KryoSerializable;
import com.esotericsoftware.kryo.io.Input;
import com.esotericsoftware.kryo.io.Output;

/**
 * A table that holds CharSequence objects for URLs.
 * <p>
 * Each URL is represented by an integer in the crawler to save memory. This
 * data structure allows tracking the relation between URLs and numbers.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class UrlTable implements KryoSerializable {
	private volatile List<Long> hashes;
	private volatile List<String> urlList;
	private volatile int pos;

	public UrlTable(int initialCapacity) {
		this.urlList = Collections.synchronizedList(new ArrayList<String>(initialCapacity));
		this.hashes = Collections.synchronizedList(new ArrayList<Long>(initialCapacity));
		this.pos = -1;
	}
	
	private UrlTable() {
		super();
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

	@Override
	public void read(Kryo arg0, Input arg1) {
		this.urlList = Collections.synchronizedList(arg0.readObject(arg1, ArrayList.class));
		this.hashes = Collections.synchronizedList(arg0.readObject(arg1, ArrayList.class));
		this.pos = arg1.readInt();
	}

	@Override
	public void write(Kryo arg0, Output arg1) {
		arg0.writeObject(arg1, new ArrayList<String>(this.urlList));
		arg0.writeObject(arg1, new ArrayList<Long>(this.hashes));
		arg1.writeInt(pos);
	}
}
