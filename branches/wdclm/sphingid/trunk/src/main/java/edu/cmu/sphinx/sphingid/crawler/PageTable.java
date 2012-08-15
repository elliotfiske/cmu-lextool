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
 * 
 * A list that contains number entries of fetched and parsed pages.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class PageTable implements KryoSerializable {
	private List<Boolean> table;

	public PageTable(int initialCapacity) {
		super();
		this.table = Collections.synchronizedList(new ArrayList<Boolean>(initialCapacity));
	}
	
	private PageTable() {
		super();
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

	@Override
	public void read(Kryo arg0, Input arg1) {
		this.table = Collections.synchronizedList(arg0.readObject(arg1, ArrayList.class));
	}

	@Override
	public void write(Kryo arg0, Output arg1) {
		arg0.writeObject(arg1, new ArrayList<Boolean>(this.table));
	}

}
