/* Copyright 1999,2004 Sun Microsystems, Inc.  
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 */
package edu.cmu.sphinx.tools.tags;

import org.mozilla.javascript.*;

import javax.speech.recognition.RuleParse;

/**
 * An ECMAScript action tags parser for tags embedded in JSGF RuleGrammars. A typical use of this class is to create an
 * instance of it and then call the parseTags method using RuleParse's generated by a RuleGrammar. The instance will
 * maintain a context/scope between calls to parseTags.
 *
 * @see #parseTags
 * @see ActionTagsUtilities
 */
public class ActionTagsParser {

    /** The ECMAScript context. */
    protected Context context;

    /** The "undefined" value in ECMAScript.  This is unique to the context. */
    protected Object undefined;

    /** The global scope. */
    protected ImporterTopLevel global;


    /**
     * Create a new ECMATagsParser.  This will generate an ECMAScript context that will be re-used each time the parse
     * method is called. After creating an instance of this class, an application will typically call the parseTags
     * method with RuleParses generated by a RuleGrammar.
     *
     * @see #parseTags
     */
    public ActionTagsParser() {
        try {
            context = Context.enter();
            context.setErrorReporter(new LocalErrorReporter());
            context.setLanguageVersion(Context.VERSION_1_2);
            global = (ImporterTopLevel)
                    context.initStandardObjects(new ImporterTopLevel(), false);
            undefined = Context.getUndefinedValue();
            String[] names = {"print", "debug"};
            global.defineFunctionProperties(names,
                    ActionTagsParser.class,
                    ScriptableObject.DONTENUM);
            context.evaluateString(global,
                    ActionTagsUtilities.getClassDefinitions(),
                    "GlobalDefinitions",
                    1,
                    null);
        } catch (JavaScriptException jse) {
            jse.printStackTrace();
        } catch (Exception e) {
        	e.printStackTrace();
        }
    }


    /**
     * Convert the tags embedded in the RuleParse to ECMAScript and then process them within the scope of this parser.
     * When the RuleParse has been parsed, the results can be obtained via the get method.
     *
     * @param ruleParse the RuleParse from a RuleGrammar.
     * @see #get
     */
    public void parseTags(RuleParse ruleParse) {
        if (ruleParse == null) {
            return;
        }

        try {
            context.evaluateString(global,
                    ActionTagsUtilities.getScript(ruleParse),
                    "parseTags",
                    1,
                    null);
        } catch (JavaScriptException jse) {
            jse.printStackTrace();
        }
    }


    /**
     * Evaluate the given ECMAScript script as ECMAScript within the context/scope of this parser.
     *
     * @param script a String containing ECMAScript to be evaluated
     * @return the result of evaluating the script
     */
    public Object evaluateString(String script) {
        Object retVal = null;
        try {
            retVal = context.evaluateString(global,
                    script,
                    "evaluateString",
                    1,
                    null);
            if (retVal == undefined) {
                retVal = null;
            }
        } catch (JavaScriptException jse) {
            jse.printStackTrace();
        }
        if ((retVal != null) && (retVal instanceof NativeJavaObject)) {
            retVal = ((NativeJavaObject) retVal).unwrap();
        }
        return retVal;
    }


    /**
     * Get the given object from the global context/scope of the parser.
     *
     * @param name the name of the object to get
     * @return the object if found; otherwise null
     * @see #parseTags
     */
    public Object getGlobal(String name) {
        return evaluateString(name + ';');
    }


    /**
     * Get the given object from the result value after a RuleParse has been parsed with the parseTags method.  For
     * example, an application might use "$value" or "$value.foo" for the name parameter.  This is basically a shortcut
     * for calling evaluateString with a parameter of "$." + name.
     *
     * @param name the name of the object to get
     * @return null if the name does not exist
     * @see #parseTags
     */
    public Object get(String name) {
        return evaluateString("$." + name + ';');
    }


    /**
     * A debug utility that can be referenced within ECMAScript source. For example, a RuleTag could contain the
     * following ECMAScript: "{ this.$value = 7; print('Set value to 7'); }".  The string will be sent to System.out.
     *
     * @param string the string to send to System.out
     */
    static public void print(String string) {
        System.out.println(string);
    }


    static java.io.BufferedReader inReader;


    /**
     * A debug utility that can be referenced within ECMAScript source. This will send the given string to System.out,
     * read a line from System.in, and then return that line.  This is to be used in conjunction with the
     * ActionTagsUtilities debugging capabilities.
     *
     * @see ActionTagsUtilities#setDebugging
     */
    static public String debug(String string) {
        System.out.println("DEBUG: " + string);
        System.out.print("DEBUG> ");
        try {
            if (inReader == null) {
                inReader = new java.io.BufferedReader(
                        new java.io.InputStreamReader(System.in));
            }
            return inReader.readLine();
        } catch (java.io.IOException e) {
            return ("step");
        }
    }


    /** Recurse through the properties in the context/scope of this parser and generate a String from them. */
    public String toString() {
        StringBuilder builder = new StringBuilder();
        printScriptable(global, builder, null);
        return builder.toString();
    }


    /** A debug utility to show the result value after a RuleParse has been parsed with the parseTags method. */
    public String showValue() {
        StringBuilder builder = new StringBuilder();
        try {
            printScriptable((Scriptable) global.get("$", global), builder, null);
        } catch (Exception e) {
            builder.append("$: unknown (").append(e).append(")\n");
            e.printStackTrace();
        }
        return builder.toString();
    }


    /** Helper method for toString and showValue. */
    protected void printScriptable(Scriptable scriptable,
                                   StringBuilder builder,
                                   String prefix) {
        if (scriptable == undefined) {
            builder.append(prefix).append(": undefined\n");
            return;
        }

        Object[] ids = scriptable.getIds();
        for (Object id : ids) {
            String idStr;
            if (prefix == null) {
                idStr = id.toString();
            } else {
                idStr = prefix + '.' + id;
            }
            try {
                Object o;
                if (id instanceof String) {
                    o = scriptable.get((String)id, scriptable);
                } else {
                    o = scriptable.get((Integer)id, scriptable);
                }
                if (o instanceof Function) {
                    //builder.append(idStr + ": function\n");
                } else if (o instanceof Scriptable) {
                    printScriptable((Scriptable)o, builder, idStr);
                } else {
                    builder.append(idStr).append(": ").append(o).append('\n');
                }
            } catch (Exception e) {
                builder.append(idStr).append(": unknown (").append(e).append(")\n");
                e.printStackTrace();
            }
        }
    }


    /** ErrorReporter to give better error information when errors are encountered while parsing action tags. */
    class LocalErrorReporter implements ErrorReporter {

        public void warning(String message, String sourceName, int line,
                            String lineSource, int lineOffset) {
            System.err.println("ECMAScript warning in " + sourceName);
            System.err.println("    line #:  " + line);
            System.err.println("    source:  " + lineSource);
            System.err.println("    message: " + message + '\n');
        }


        public void error(String message, String sourceName, int line,
                          String lineSource, int lineOffset) {
            System.err.println("ECMAScript error in " + sourceName);
            System.err.println("    line #:  " + line);
            System.err.println("    source:  " + lineSource);
            System.err.println("    message: " + message + '\n');
            throw new EvaluatorException(message);
        }


        public EvaluatorException runtimeError(String message, String sourceName,
                                               int line, String lineSource,
                                               int lineOffset) {
            return new EvaluatorException(message);
        }
    }
}
