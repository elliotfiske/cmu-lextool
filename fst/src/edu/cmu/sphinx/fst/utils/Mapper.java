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

import java.util.Collection;
import java.util.HashMap;
import java.util.Set;

/**
 * 
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Mapper<K, V> {

    private HashMap<K, V> map;
    private HashMap<V, K> reverse;

    /**
     * Default Constructor
     */
    public Mapper() {
        map = new HashMap<K, V>();
        reverse = new HashMap<V, K>();
    }

    public V put(K key, V value) {
        reverse.put(value, key);
        return map.put(key, value);

    }

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
        return reverse.get(value);
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
            reverse.remove(value);
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
            reverse.remove(value);
        }
        return value;
    }

    public Set<K> keySet() {
        return map.keySet();
    }

    public Collection<V> valueSet() {
        return map.values();
    }

    /*
     * (non-Javadoc)
     * 
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
        } else if (!map.equals(other.map))
            return false;
        if (reverse == null) {
            if (other.reverse != null)
                return false;
        } else if (!reverse.equals(other.reverse))
            return false;
        return true;
    }

    /*
     * (non-Javadoc)
     * 
     * @see java.lang.Object#toString()
     */
    @Override
    public String toString() {
        return "Map [map=" + map + "]";
    }

}
