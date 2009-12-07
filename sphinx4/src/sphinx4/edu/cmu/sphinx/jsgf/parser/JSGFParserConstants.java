/* Generated By:JavaCC: Do not edit this line. JSGFParserConstants.java */
package edu.cmu.sphinx.jsgf.parser;


/**
 * Token literal values and constants.
 * Generated by org.javacc.parser.OtherFilesGen#start()
 */
public interface JSGFParserConstants {

  /** End of File. */
  int EOF = 0;
  /** RegularExpression Id. */
  int SINGLE_LINE_COMMENT = 9;
  /** RegularExpression Id. */
  int FORMAL_COMMENT = 10;
  /** RegularExpression Id. */
  int MULTI_LINE_COMMENT = 11;
  /** RegularExpression Id. */
  int GRAMMAR = 13;
  /** RegularExpression Id. */
  int IMPORT = 14;
  /** RegularExpression Id. */
  int PUBLIC = 15;
  /** RegularExpression Id. */
  int INTEGER_LITERAL = 16;
  /** RegularExpression Id. */
  int DECIMAL_LITERAL = 17;
  /** RegularExpression Id. */
  int FLOATING_POINT_LITERAL = 18;
  /** RegularExpression Id. */
  int EXPONENT = 19;
  /** RegularExpression Id. */
  int CHARACTER_LITERAL = 20;
  /** RegularExpression Id. */
  int STRING_LITERAL = 21;
  /** RegularExpression Id. */
  int TAG = 22;
  /** RegularExpression Id. */
  int IDENTIFIER = 23;
  /** RegularExpression Id. */
  int LETTER = 24;
  /** RegularExpression Id. */
  int DIGIT = 25;

  /** Lexical state. */
  int DEFAULT = 0;
  /** Lexical state. */
  int IN_SINGLE_LINE_COMMENT = 1;
  /** Lexical state. */
  int IN_FORMAL_COMMENT = 2;
  /** Lexical state. */
  int IN_MULTI_LINE_COMMENT = 3;

  /** Literal token values. */
  String[] tokenImage = {
    "<EOF>",
    "\" \"",
    "\"\\t\"",
    "\"\\n\"",
    "\"\\r\"",
    "\"\\f\"",
    "\"//\"",
    "<token of kind 7>",
    "\"/*\"",
    "<SINGLE_LINE_COMMENT>",
    "\"*/\"",
    "\"*/\"",
    "<token of kind 12>",
    "\"grammar\"",
    "\"import\"",
    "\"public\"",
    "<INTEGER_LITERAL>",
    "<DECIMAL_LITERAL>",
    "<FLOATING_POINT_LITERAL>",
    "<EXPONENT>",
    "<CHARACTER_LITERAL>",
    "<STRING_LITERAL>",
    "<TAG>",
    "<IDENTIFIER>",
    "<LETTER>",
    "<DIGIT>",
    "\";\"",
    "\"V1.0\"",
    "\"<\"",
    "\".\"",
    "\"*\"",
    "\">\"",
    "\"=\"",
    "\"|\"",
    "\"/\"",
    "\"+\"",
    "\"(\"",
    "\")\"",
    "\"[\"",
    "\"]\"",
  };

}
