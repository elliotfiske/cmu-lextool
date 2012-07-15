/**
 * 
 * Copyright 1999-2012 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.fst.utils;

import java.io.Serializable;
import java.util.Set;

import com.google.common.collect.HashBiMap;

/**
 * A one-to-one map generic class the wraps around
 * @see com.google.common.collect.AbstractBiMap 
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Mapper<K, V> implements Serializable {

	private static final long serialVersionUID = -6780807561998593228L;

	private HashBiMap<K, V> map;

	/**
	 * Default Constructor
	 */
	public Mapper() {
		map = HashBiMap.create();
	}
	
	/*
	 * (non-Javadoc)
	 * @see com.google.common.collect.HashBiMap#put(K key, V value)
	 */
	public V put(K key, V value) {
		return map.put(key, value); 
		
	}
	
	/*
	 * (non-Javadoc)
	 * see com.google.common.collect.ForwardingMap.get(java.lang.Object)
	 */
	public V getValue(K key) {
		return map.get(key);
	}
	/**
	 * Gets the key that corresponds to a value 
	 * 
	 * @param value the value
	 * @return the corresponding key
	 */
	public K getKey(V value) {
		return map.inverse().get(value);
	}
	
	/**
	 * Gets the map's size
	 * 
	 * @return the map's size
	 */
	public int size() {
		return map.size();
	}

	/**
	 * Removes a value from the map
	 * 
	 * @param value the value to remove
	 * @return the jey of the removed value
	 */
	public K removeValue(V value) {
		K key = this.getKey(value);
		if (key != null) {
			map.remove(key);
		}
		return key;
	}
	
	/**
	 * Removes a key from the map
	 * 
	 * @param key the key to remove
	 * @return the value of the removed key
	 */
	public V removeKey(K key) {
		V value = null;
		if (key != null) {
			value = map.get(key);
			map.remove(key);
		}
		return value;
	}

	/*
	 * (non-Javadoc)
	 * @see com.google.common.collect.AbstractBiMap#keySet()
	 */
	public Set<K> keySet() {
		return map.keySet();
	}
	
	/*
	 * (non-Javadoc)
	 * @see com.google.common.collect.AbstractBiMap#values()
	 */
	public Set<V> valueSet() {
		return map.values();
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj) {
		if (this == obj)
			return true;
		if (obj == null)
			return false;
		if (getClass() != obj.getClass())
			return false;
		@SuppressWarnings("unchecked")
		Mapper<K, V> other = (Mapper<K, V>) obj;
		if (map == null) {
			if (other.map != null)
				return false;
		} else 
			if (!map.equals(other.map))
			return false;
		return true;
	}
	
	/* (non-Javadoc)
	 * @see java.lang.Object#toString()
	 */
	@Override
	public String toString() {
		return "Map [map=" + map + "]";
	}

	
}
