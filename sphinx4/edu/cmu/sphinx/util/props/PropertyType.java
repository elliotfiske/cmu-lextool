/*
 * 
 * Copyright 1999-2004 Carnegie Mellon University.  
 * Portions Copyright 2004 Sun Microsystems, Inc.  
 * Portions Copyright 2004 Mitsubishi Electronic Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */
package edu.cmu.sphinx.util.props;

import java.util.List;

/**
 * an enum-style type that defines the possible property types.
 */
public abstract class PropertyType {
    /**
     * An integer type
     */
    public static PropertyType INT = new PropertyType("int") {
        /**
         * Determines if this is a valid integer
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid integer
         */
        boolean isValid(Object obj) {
            if (obj instanceof String) {
                String value = (String) obj;
                try {
                    Integer.parseInt(value);
                    return true;
                } catch (NumberFormatException nfe) {
                    return false;
                }
            }
            return false;
        }
    };
    
    /**
     * An integer type
     */
    public static PropertyType BOOLEAN = new PropertyType("boolean") {
        /**
         * Determines if this is a valid integer
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid integer
         */
        boolean isValid(Object obj) {
            if (obj instanceof String) {
                String value = (String) obj;
                return value.equalsIgnoreCase("true") ||
                       value.equalsIgnoreCase("false");
            }
            return false;
        }
    };
    /**
     * A floating point type
     */
    public static PropertyType FLOAT = new PropertyType("float") {
        /**
         * Determines if this is a valid float
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid float
         */
        boolean isValid(Object obj) {
            if (obj instanceof String) {
                String value = (String) obj;
                try {
                    Float.parseFloat(value);
                    return true;
                } catch (NumberFormatException nfe) {
                    return false;
                }
            }
            return false;
        }
    };
    
    /**
     * A floating point type
     */
    public static PropertyType DOUBLE = new PropertyType("double") {
        /**
         * Determines if this is a valid double
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid double
         */
        boolean isValid(Object obj) {
            if (obj instanceof String) {
                String value = (String) obj;
                try {
                    Double.parseDouble(value);
                    return true;
                } catch (NumberFormatException nfe) {
                    return false;
                }
            }
            return false;
        }
    };
    
    /**
     * A String type
     */
    public static PropertyType COMPONENT = new PropertyType("Component") {
        /**
         * Determines if this is a valid component name. Currently, all strings
         * are considered to be valid components.
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid component.
         */
        boolean isValid(Object obj) {
            return obj instanceof String;
        }
    };
    
    /**
     * An array of components
     */
    public static PropertyType COMPONENT_LIST = new PropertyType("ComponentList") {
        /**
         * Determines if this is a valid component list
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid string array
         */
        boolean isValid(Object obj) {
            return obj instanceof List;
        }
    };
    /**
     * A String type
     */
    public static PropertyType STRING = new PropertyType("String") {
        /**
         * Determines if this is a valid string
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid string
         */
        boolean isValid(Object obj) {
            return obj instanceof String;
        }
    };
    /**
     * An array of strings
     */
    public static PropertyType STRING_LIST = new PropertyType("StringList") {
        /**
         * Determines if this is a valid string array
         * 
         * @param obj
         *            the object to check
         * @return true if the object is a valid string array
         */
        boolean isValid(Object obj) {
            return obj instanceof List;
        }
    };
    private String name;
    /**
     * Creates a property type.
     * 
     * @param name
     *            the name of the property type
     */
    private PropertyType(String name) {
        this.name = name;
    }
    public String toString() {
        return name;
    }
    /**
     * Determines if the given object can be converted to this type. For
     * non-array types this is String. For array types (String Array) this a
     * String[].
     * 
     * @param obj
     *            the object to verify
     * @return true if the object can be converted to an object of this type.
     */
    abstract boolean isValid(Object obj);
}
