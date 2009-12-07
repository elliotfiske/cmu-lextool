/* Generated By:JavaCC: Do not edit this line. JSGFParser.java */
package com.sun.speech.engine.recognition.parser;

import java.io.BufferedInputStream;
import java.io.ByteArrayInputStream;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.Reader;
import java.io.StringReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.StringTokenizer;

import javax.speech.recognition.*;
import com.sun.speech.engine.recognition.BaseRuleGrammar;
import com.sun.speech.engine.recognition.RuleGrammarFactory;
import com.sun.speech.engine.recognition.BaseRuleGrammarFactory;

class JSGFEncoding {
    public String version;

    public String encoding;

    public String locale;

    JSGFEncoding(String version, String encoding, String locale) {
        this.version = version;
        this.encoding = encoding;
        this.locale = locale;
    }
}

public class JSGFParser implements JSGFParserConstants {
    // For now we create one global parser, if needed JavaCC can be set
    // to allow the creation of multiple parser instances
    //
    static final String version = "1.0";

    static JSGFParser parser = null;

    // This main method simply checks the syntax of a jsgf Grammar
    //
    public static void main(String args []) {
        if (args.length == 0) {
            System.out.println("JSGF Parser Version " + version + ":  Reading from standard input . . .");
            parser = new JSGFParser(System.in);
        }
        else if (args.length > 0) {
            System.out.println("JSGF Parser Version " + version + ":  Reading from file " + args [0] + " . . .");
            try {
                URL codeBase = null;
                File f = new File(".");
                String path = f.getAbsolutePath() + "/" + args [0];
                try {
                    codeBase = new URL("file:" + path);
                } catch (MalformedURLException e) {
                    System.out.println("Could not get URL for current directory " + e);
                    return;
                }
                BufferedInputStream i = new BufferedInputStream(codeBase.openStream(), 256);
                JSGFEncoding encoding = getJSGFEncoding(i);
                Reader rdr;
                if ((encoding != null) && (encoding.encoding != null)) {
                    System.out.println("Grammar Character Encoding \u005c"" + encoding.encoding + "\u005c"");
                    rdr = new InputStreamReader(i, encoding.encoding);
                } else {
                    if (encoding == null) System.out.println("WARNING: Grammar missing self identifying header");
                    rdr = new InputStreamReader(i);
                }
                parser = new JSGFParser(rdr);
            } catch (Exception e) {
                System.out.println("JSGF Parser Version " + version + ":  File " + args [0] + " not found.");
                return;
            }
        } else {
            System.out.println("JSGF Parser Version " + version + ":  Usage is one of:");
            System.out.println("         java JSGFParser < inputfile");
            System.out.println("OR");
            System.out.println("         java JSGFParser inputfile");
            return;
        }
        try {
            parser.GrammarUnit(new BaseRuleGrammarFactory (null));
            System.out.println("JSGF Parser Version " + version + ":  JSGF Grammar parsed successfully.");
        } catch (ParseException e) {
            System.out.println("JSGF Parser Version " + version + ":  Encountered errors during parse." + e.getMessage());
        }
    }

    /**
     * newGrammarFromJSGF - Once JavaCC supports Readers we will change this
     */
    public static RuleGrammar newGrammarFromJSGF(InputStream i, RuleGrammarFactory factory) throws GrammarParseException
    {
        RuleGrammar G = null;
        if (parser == null) {
            parser = new JSGFParser(i);
        } else {
            parser.ReInit(i);
        }
        try {
            G = parser.GrammarUnit(factory);
            return G;
        } catch (ParseException e) {
            Token etoken = e.currentToken;
            GrammarParseException ge = new GrammarParseException(etoken.beginLine, etoken.beginColumn, "Grammar Error", e.getMessage());
            throw ge;
        }
    }

    /**
     * newGrammarFromJSGF - Once JavaCC supports Readers we will change this
     */
    public static RuleGrammar newGrammarFromJSGF(Reader i, RuleGrammarFactory factory) throws GrammarParseException
    {
        RuleGrammar G = null;
        if (parser == null) {
            parser = new JSGFParser(i);
        } else {
            parser.ReInit(i);
        }
        try {
            G = parser.GrammarUnit(factory);
            return G;
        } catch (ParseException e){
            Token etoken = e.currentToken;
            GrammarParseException ge = new GrammarParseException(etoken.beginLine, etoken.beginColumn, "Grammar Error", e.getMessage());
            throw ge;
        }
    }

    private static JSGFEncoding getJSGFEncoding(BufferedInputStream is) {
        int i = 0;
        byte b [] = new byte [2];
        byte c [] = new byte [80];
        is.mark(256);
        /* read 2 bytes */
        try {
            if (is.read(b, 0, 2) != 2) {
                is.reset();
                return null;
            }
            // UTF-8
            if ((b [0] == 0x23) && (b [1] == 0x4A)) {
                i = 0;
                c [i++] = b [0];
                c [i++] = b [1];
                while (i < 80) {
                    if (is.read(b, 0, 1) != 1) {
                        is.reset();
                        return null;
                    }
                    if ((b [0] == 0x0A) || (b [0] == 0x0D)) break;
                    c [i++] = b [0];
                }
            } else if ((b [0] == 0x23) && (b [1] == 0x00)) {
                // UTF-16 BE
                i = 0;
                c [i++] = b [0];
                while (i < 80) {
                    if (is.read(b, 0, 2) != 2) {
                        is.reset();
                        return null;
                    }
                    if (b [1] != 0) return null;
                    if ((b [0] == 0x0A) || (b [0] == 0x0D)) break;
                    c [i++] = b [0];
                }
             } else {
                // UTF-16 LE
                if ((b [0] == 0x00) && (b [1] == 0x23)) {
                    i = 0;
                    c [i++] = b [1];
                    while (i < 80) {
                        if (is.read(b, 0, 2) != 2) {
                            is.reset();
                            return null;
                        }
                        if (b [0] != 0) return null;
                        if ((b [1] == 0x0A) || (b [1] == 0x0D)) break;
                        c [i++] = b [1];
                    }
                }
            }
        }
        catch (IOException ioe) {
            try {
                is.reset();
            } catch (IOException ioe2) {
            }
            return null;
        }
        if (i == 0) {
            try {
                is.reset();
            } catch (IOException ioe2) {
            }
            return null;
        }
        //
        // Now c[] should have first line of text in UTF-8 format
        //
        String estr = new String(c, 0, i);
        StringTokenizer st = new StringTokenizer(estr, " \u005ct\u005cn\u005cr\u005cf;");
        String id = null;
        String ver = null;
        String enc = null;
        String loc = null;
        if (st.hasMoreTokens()) id = st.nextToken();
        if (!id.equals("#JSGF")) {
            try {
                is.reset();
            } catch (IOException ioe2) {
            }
            return null;
        }
        if (st.hasMoreTokens()) ver = st.nextToken();
        if (st.hasMoreTokens()) enc = st.nextToken();
        if (st.hasMoreTokens()) loc = st.nextToken();
        return new JSGFEncoding(ver, enc, loc);
    }

    /**
     * newGrammarFromURL
     */
    public static RuleGrammar newGrammarFromJSGF(URL url, RuleGrammarFactory factory) throws GrammarParseException, IOException
    {
        Reader reader;
        BufferedInputStream stream = new BufferedInputStream(url.openStream(), 256);
        JSGFEncoding encoding = getJSGFEncoding(stream);
        if ((encoding != null) && (encoding.encoding != null)) {
            System.out.println("Grammar Character Encoding \u005c"" + encoding.encoding + "\u005c"");
            reader = new InputStreamReader(stream, encoding.encoding);
        } else {
            if (encoding == null) System.out.println("WARNING: Grammar missing self identifying header");
            reader = new InputStreamReader(stream);
        }
        return newGrammarFromJSGF(reader, factory);
    }

    /**
     * ruleForJSGF
     */
    public static Rule ruleForJSGF(String text) {
        Rule r = null;
        try {
            StringReader sread = new StringReader(text);
            if (parser == null) parser = new JSGFParser(sread);
            else parser.ReInit(sread);
            r = parser.alternatives();
            // System.out.println("JSGF Parser Version " + version
            //                    + ":  JSGF RHS parsed successfully.");
        } catch (ParseException e) {
            System.out.println("JSGF Parser Version " + version + ":  Encountered errors during parse.");
        }
        return r;
    }

    /**
     * Parse an apparent rulename reference.
     */
    public static RuleName parseRuleName(String text) throws GrammarParseException {
        RuleName r = null;
        try {
            ByteArrayInputStream stream = new ByteArrayInputStream(text.getBytes());
            if (parser == null) parser = new JSGFParser(stream);
            else parser.ReInit(stream);
            r = parser.ruleRef();
        }
        catch (ParseException e) {
            throw new GrammarParseException("JSGF Parser Version " + version + " error");
        }
        return r;
    }

    /**
     * Parse and apparent import declaration
     */
    public static RuleName parseImport(String text) throws GrammarParseException {
        RuleName r = null;
        try {
            ByteArrayInputStream stream = new ByteArrayInputStream(text.getBytes());
            if (parser == null) parser = new JSGFParser(stream);
            else parser.ReInit(stream);
            r = parser.importRef();
        } catch (ParseException e) {
            throw new GrammarParseException("JSGF Parser Version " + version + " error");
        }
        return r;
    }

    /**
    * extract @xxxx keywords from documention comments
    */
    static void extractKeywords(BaseRuleGrammar grammar, String rname, String comment) {
        int i = 0;
        while ((i = comment.indexOf("@example ", i) + 9) > 9) {
            int j = Math.max(comment.indexOf('\u005cr', i), comment.indexOf('\u005cn', i));
            if (j < 0) {
                j = comment.length();
                if (comment.endsWith(("*/")))
                    j -= 2;
            }
            grammar.addSampleSentence(rname, comment.substring(i, j).trim());
            i = j + 1;
        }
    }

  final public RuleGrammar GrammarUnit(RuleGrammarFactory factory) throws ParseException {
    RuleGrammar G = null;
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case IDENTIFIER:
      IdentHeader();
      break;
    default:
      jj_la1[0] = jj_gen;
      ;
    }
    G = GrammarDeclaration(factory);
    label_1:
    while (true) {
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case IMPORT:
        ;
        break;
      default:
        jj_la1[1] = jj_gen;
        break label_1;
      }
      ImportDeclaration(G);
    }
    label_2:
    while (true) {
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case PUBLIC:
      case 28:
        ;
        break;
      default:
        jj_la1[2] = jj_gen;
        break label_2;
      }
      RuleDeclaration(G);
    }
    jj_consume_token(0);
        {if (true) return G;}
    throw new Error("Missing return statement in function");
  }

  final public RuleGrammar GrammarDeclaration(RuleGrammarFactory factory) throws ParseException {
    String s;
    RuleGrammar G = null;
    Token t = null;
    t = jj_consume_token(GRAMMAR);
    s = Name();
    jj_consume_token(26);
        G = factory.newGrammar (s);
        if (G != null && t != null && t.specialToken != null) {
            if (t.specialToken.image != null && t.specialToken.image.startsWith("/**")) {
                BaseRuleGrammar JG = (BaseRuleGrammar) G;
                JG.addGrammarDocComment(t.specialToken.image);
            }
        }
        {if (true) return G;}
    throw new Error("Missing return statement in function");
  }

  final public void IdentHeader() throws ParseException {
    jj_consume_token(IDENTIFIER);
    jj_consume_token(27);
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case IDENTIFIER:
      jj_consume_token(IDENTIFIER);
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case IDENTIFIER:
        jj_consume_token(IDENTIFIER);
        break;
      default:
        jj_la1[3] = jj_gen;
        ;
      }
      break;
    default:
      jj_la1[4] = jj_gen;
      ;
    }
    jj_consume_token(26);
  }

  final public void ImportDeclaration(RuleGrammar G) throws ParseException {
    boolean all = false;
    String name;
    Token t = null;
    t = jj_consume_token(IMPORT);
    jj_consume_token(28);
    name = Name();
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case 29:
      jj_consume_token(29);
      jj_consume_token(30);
            all = true;
      break;
    default:
      jj_la1[5] = jj_gen;
      ;
    }
    jj_consume_token(31);
    jj_consume_token(26);
        // import all rules if .*
        if (all) name = name + ".*";
        RuleName r = new RuleName(name);
        if (G != null) {
            G.addImport(r);
            if (G instanceof BaseRuleGrammar && t != null && t.specialToken != null) {
                if (t.specialToken.image != null && t.specialToken.image.startsWith("/**")) {
                    BaseRuleGrammar JG = (BaseRuleGrammar) G;
                    JG.addImportDocComment(r, t.specialToken.image);
                }
            }
        }
  }

  final public String Name() throws ParseException {
    Token t1, t2;
    StringBuffer sb = new StringBuffer();
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case IDENTIFIER:
      t1 = jj_consume_token(IDENTIFIER);
      break;
    case PUBLIC:
      t1 = jj_consume_token(PUBLIC);
      break;
    case IMPORT:
      t1 = jj_consume_token(IMPORT);
      break;
    case GRAMMAR:
      t1 = jj_consume_token(GRAMMAR);
      break;
    default:
      jj_la1[6] = jj_gen;
      jj_consume_token(-1);
      throw new ParseException();
    }
        sb.append(t1.image);
    label_3:
    while (true) {
      if (jj_2_1(2)) {
        ;
      } else {
        break label_3;
      }
      jj_consume_token(29);
      t2 = jj_consume_token(IDENTIFIER);
            sb.append('.');
            sb.append(t2.image);
    }
        {if (true) return sb.toString();}
    throw new Error("Missing return statement in function");
  }

  final public void RuleDeclaration(RuleGrammar G) throws ParseException {
    boolean pub = false;
    String s;
    Rule r;
    Token t = null;
    Token t1 = null;
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case PUBLIC:
      t = jj_consume_token(PUBLIC);
            pub = true;
      break;
    default:
      jj_la1[7] = jj_gen;
      ;
    }
    t1 = jj_consume_token(28);
    s = ruleDef();
    jj_consume_token(31);
    jj_consume_token(32);
    r = alternatives();
    jj_consume_token(26);
        try {
            if (G != null) {
                G.setRule(s, r, pub);
                if (G instanceof BaseRuleGrammar) {
                    BaseRuleGrammar JG = (BaseRuleGrammar) G;
                    String docComment = null;
                    if ((t != null) && (t.specialToken != null) && (t.specialToken.image != null)) docComment = t.specialToken.image;
                    else if ((t1 != null) && (t1.specialToken != null) && (t1.specialToken.image != null)) docComment = t1.specialToken.image;
                    if (docComment != null && docComment.startsWith("/**")) {
                        extractKeywords(JG, s, docComment);
                        JG.addRuleDocComment(s, docComment);
                    }
                    JG.setSourceLine(s, t1.beginLine);
                }
            }
        } catch (IllegalArgumentException e) {
            System.out.println("ERROR SETTING RULE " + s);
        }
  }

  final public RuleAlternatives alternatives() throws ParseException {
    ArrayList < Rule > ruleList = new ArrayList < Rule > ();
    Rule r;
    float w;
    float wa [ ] = new float [ 25 ];
    int cnt = 0;
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case GRAMMAR:
    case IMPORT:
    case PUBLIC:
    case INTEGER_LITERAL:
    case STRING_LITERAL:
    case IDENTIFIER:
    case 28:
    case 36:
    case 38:
      r = sequence();
                           ruleList.add(r);
      label_4:
      while (true) {
        switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
        case 33:
          ;
          break;
        default:
          jj_la1[8] = jj_gen;
          break label_4;
        }
        jj_consume_token(33);
        r = sequence();
                                                                     ruleList.add(r);
      }
      break;
    case 34:
      w = weight();
      r = sequence();
                ruleList.add(r);
                wa [ cnt++ ] = w;
      label_5:
      while (true) {
        jj_consume_token(33);
        w = weight();
        r = sequence();
                    ruleList.add(r);
                    // make array bigger if needed
                    if (cnt > (wa.length - 1)) {
                        float watmp [ ] = new float [ wa.length + 25 ];
                        System.arraycopy(wa, 0, watmp, 0, wa.length);
                        wa = watmp;
                    }
                    wa [ cnt++ ] = w;
        switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
        case 33:
          ;
          break;
        default:
          jj_la1[9] = jj_gen;
          break label_5;
        }
      }
      break;
    default:
      jj_la1[10] = jj_gen;
      jj_consume_token(-1);
      throw new ParseException();
    }
        Rule rarry [ ] = new Rule [ ruleList.size() ];
        ruleList.toArray(rarry);
        RuleAlternatives ra = new RuleAlternatives(rarry);
        if (cnt != 0) {
            float wa1 [ ] = new float [ cnt ];
            System.arraycopy(wa, 0, wa1, 0, cnt);
            try {
                ra.setWeights(wa1);
            } catch (IllegalArgumentException e) {
                System.out.println("ERROR " + e);
            }
        }
        {if (true) return ra;}
    throw new Error("Missing return statement in function");
  }

  final public String ruleDef() throws ParseException {
    Token t;
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case IDENTIFIER:
      t = jj_consume_token(IDENTIFIER);
      break;
    case INTEGER_LITERAL:
      t = jj_consume_token(INTEGER_LITERAL);
      break;
    case PUBLIC:
      t = jj_consume_token(PUBLIC);
      break;
    case IMPORT:
      t = jj_consume_token(IMPORT);
      break;
    case GRAMMAR:
      t = jj_consume_token(GRAMMAR);
      break;
    default:
      jj_la1[11] = jj_gen;
      jj_consume_token(-1);
      throw new ParseException();
    }
        {if (true) return t.image;}
    throw new Error("Missing return statement in function");
  }

  final public RuleSequence sequence() throws ParseException {
    Rule rule;
    ArrayList < Rule > ruleList = new ArrayList < Rule > ();
    label_6:
    while (true) {
      rule = item();
            ruleList.add(rule);
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case GRAMMAR:
      case IMPORT:
      case PUBLIC:
      case INTEGER_LITERAL:
      case STRING_LITERAL:
      case IDENTIFIER:
      case 28:
      case 36:
      case 38:
        ;
        break;
      default:
        jj_la1[12] = jj_gen;
        break label_6;
      }
    }
        Rule ruleArray [ ] = new Rule [ ruleList.size() ];
        ruleList.toArray(ruleArray);
        {if (true) return new RuleSequence(ruleArray);}
    throw new Error("Missing return statement in function");
  }

  final public float weight() throws ParseException {
    Token t;
    jj_consume_token(34);
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case FLOATING_POINT_LITERAL:
      t = jj_consume_token(FLOATING_POINT_LITERAL);
      break;
    case INTEGER_LITERAL:
      t = jj_consume_token(INTEGER_LITERAL);
      break;
    default:
      jj_la1[13] = jj_gen;
      jj_consume_token(-1);
      throw new ParseException();
    }
    jj_consume_token(34);
        {if (true) return Float.valueOf(t.image).floatValue();}
    throw new Error("Missing return statement in function");
  }

  final public Rule item() throws ParseException {
    Rule r;
    ArrayList < String > tags = null;
    int count =-1;
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case GRAMMAR:
    case IMPORT:
    case PUBLIC:
    case INTEGER_LITERAL:
    case STRING_LITERAL:
    case IDENTIFIER:
    case 28:
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case GRAMMAR:
      case IMPORT:
      case PUBLIC:
      case INTEGER_LITERAL:
      case STRING_LITERAL:
      case IDENTIFIER:
        r = terminal();
        break;
      case 28:
        r = ruleRef();
        break;
      default:
        jj_la1[14] = jj_gen;
        jj_consume_token(-1);
        throw new ParseException();
      }
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case 30:
      case 35:
        switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
        case 30:
          jj_consume_token(30);
                    count = RuleCount.ZERO_OR_MORE;
          break;
        case 35:
          jj_consume_token(35);
                    count = RuleCount.ONCE_OR_MORE;
          break;
        default:
          jj_la1[15] = jj_gen;
          jj_consume_token(-1);
          throw new ParseException();
        }
        break;
      default:
        jj_la1[16] = jj_gen;
        ;
      }
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case TAG:
        tags = tags();
        break;
      default:
        jj_la1[17] = jj_gen;
        ;
      }
      break;
    case 36:
      jj_consume_token(36);
      r = alternatives();
      jj_consume_token(37);
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case 30:
      case 35:
        switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
        case 30:
          jj_consume_token(30);
                    count = RuleCount.ZERO_OR_MORE;
          break;
        case 35:
          jj_consume_token(35);
                                                              count = RuleCount.ONCE_OR_MORE;
          break;
        default:
          jj_la1[18] = jj_gen;
          jj_consume_token(-1);
          throw new ParseException();
        }
        break;
      default:
        jj_la1[19] = jj_gen;
        ;
      }
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case TAG:
        tags = tags();
        break;
      default:
        jj_la1[20] = jj_gen;
        ;
      }
      break;
    case 38:
      jj_consume_token(38);
      r = alternatives();
      jj_consume_token(39);
                                         count = RuleCount.OPTIONAL;
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case TAG:
        tags = tags();
        break;
      default:
        jj_la1[21] = jj_gen;
        ;
      }
      break;
    default:
      jj_la1[22] = jj_gen;
      jj_consume_token(-1);
      throw new ParseException();
    }
        if (count != - 1) r = new RuleCount(r, count);
        if (tags != null) {
            for (String tag : tags) {
                if (tag.charAt(0) == '{') {
                    tag = tag.substring(1, tag.length() - 1);
                    tag = tag.replace('\u005c\u005c', ' ');
                }
                r = new RuleTag(r, tag);
            }
        }
        {if (true) return r;}
    throw new Error("Missing return statement in function");
  }

  final public ArrayList < String > tags() throws ParseException {
    Token token;
    ArrayList < String > tags = new ArrayList < String > ();
    label_7:
    while (true) {
      token = jj_consume_token(TAG);
                        tags.add(token.image);
      switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
      case TAG:
        ;
        break;
      default:
        jj_la1[23] = jj_gen;
        break label_7;
      }
    }
        {if (true) return tags;}
    throw new Error("Missing return statement in function");
  }

  final public Rule terminal() throws ParseException {
    Token t;
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case IDENTIFIER:
      t = jj_consume_token(IDENTIFIER);
      break;
    case STRING_LITERAL:
      t = jj_consume_token(STRING_LITERAL);
      break;
    case INTEGER_LITERAL:
      t = jj_consume_token(INTEGER_LITERAL);
      break;
    case PUBLIC:
      t = jj_consume_token(PUBLIC);
      break;
    case IMPORT:
      t = jj_consume_token(IMPORT);
      break;
    case GRAMMAR:
      t = jj_consume_token(GRAMMAR);
      break;
    default:
      jj_la1[24] = jj_gen;
      jj_consume_token(-1);
      throw new ParseException();
    }
        String tn = t.image;
        if (tn.startsWith("\u005c"") && tn.endsWith("\u005c"")) tn = tn.substring(1, tn.length() - 1);
        RuleToken rt = new RuleToken(tn);
        {if (true) return rt;}
    throw new Error("Missing return statement in function");
  }

  final public RuleName ruleRef() throws ParseException {
    String s;
    jj_consume_token(28);
    s = Name();
    jj_consume_token(31);
        RuleName rn = new RuleName(s);
        {if (true) return rn;}
    throw new Error("Missing return statement in function");
  }

  final public RuleName importRef() throws ParseException {
    String s;
    boolean all = false;
    jj_consume_token(28);
    s = Name();
    switch ((jj_ntk==-1)?jj_ntk():jj_ntk) {
    case 29:
      jj_consume_token(29);
      jj_consume_token(30);
                                 all = true;
      break;
    default:
      jj_la1[25] = jj_gen;
      ;
    }
    jj_consume_token(31);
        if (all) s = s + ".*";
        RuleName rn = new RuleName(s);
        {if (true) return rn;}
    throw new Error("Missing return statement in function");
  }

  private boolean jj_2_1(int xla) {
    jj_la = xla; jj_lastpos = jj_scanpos = token;
    try { return !jj_3_1(); }
    catch(LookaheadSuccess ls) { return true; }
    finally { jj_save(0, xla); }
  }

  private boolean jj_3_1() {
    if (jj_scan_token(29)) return true;
    if (jj_scan_token(IDENTIFIER)) return true;
    return false;
  }

  /** Generated Token Manager. */
  public JSGFParserTokenManager token_source;
  JavaCharStream jj_input_stream;
  /** Current token. */
  public Token token;
  /** Next token. */
  public Token jj_nt;
  private int jj_ntk;
  private Token jj_scanpos, jj_lastpos;
  private int jj_la;
  private int jj_gen;
  final private int[] jj_la1 = new int[26];
  static private int[] jj_la1_0;
  static private int[] jj_la1_1;
  static {
      jj_la1_init_0();
      jj_la1_init_1();
   }
   private static void jj_la1_init_0() {
      jj_la1_0 = new int[] {0x800000,0x4000,0x10008000,0x800000,0x800000,0x20000000,0x80e000,0x8000,0x0,0x0,0x10a1e000,0x81e000,0x10a1e000,0x50000,0x10a1e000,0x40000000,0x40000000,0x400000,0x40000000,0x40000000,0x400000,0x400000,0x10a1e000,0x400000,0xa1e000,0x20000000,};
   }
   private static void jj_la1_init_1() {
      jj_la1_1 = new int[] {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x2,0x2,0x54,0x0,0x50,0x0,0x0,0x8,0x8,0x0,0x8,0x8,0x0,0x0,0x50,0x0,0x0,0x0,};
   }
  final private JJCalls[] jj_2_rtns = new JJCalls[1];
  private boolean jj_rescan = false;
  private int jj_gc = 0;

  /** Constructor with InputStream. */
  public JSGFParser(java.io.InputStream stream) {
     this(stream, null);
  }
  /** Constructor with InputStream and supplied encoding */
  public JSGFParser(java.io.InputStream stream, String encoding) {
    try { jj_input_stream = new JavaCharStream(stream, encoding, 1, 1); } catch(java.io.UnsupportedEncodingException e) { throw new RuntimeException(e); }
    token_source = new JSGFParserTokenManager(jj_input_stream);
    token = new Token();
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 26; i++) jj_la1[i] = -1;
    for (int i = 0; i < jj_2_rtns.length; i++) jj_2_rtns[i] = new JJCalls();
  }

  /** Reinitialise. */
  public void ReInit(java.io.InputStream stream) {
     ReInit(stream, null);
  }
  /** Reinitialise. */
  public void ReInit(java.io.InputStream stream, String encoding) {
    try { jj_input_stream.ReInit(stream, encoding, 1, 1); } catch(java.io.UnsupportedEncodingException e) { throw new RuntimeException(e); }
    token_source.ReInit(jj_input_stream);
    token = new Token();
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 26; i++) jj_la1[i] = -1;
    for (int i = 0; i < jj_2_rtns.length; i++) jj_2_rtns[i] = new JJCalls();
  }

  /** Constructor. */
  public JSGFParser(java.io.Reader stream) {
    jj_input_stream = new JavaCharStream(stream, 1, 1);
    token_source = new JSGFParserTokenManager(jj_input_stream);
    token = new Token();
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 26; i++) jj_la1[i] = -1;
    for (int i = 0; i < jj_2_rtns.length; i++) jj_2_rtns[i] = new JJCalls();
  }

  /** Reinitialise. */
  public void ReInit(java.io.Reader stream) {
    jj_input_stream.ReInit(stream, 1, 1);
    token_source.ReInit(jj_input_stream);
    token = new Token();
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 26; i++) jj_la1[i] = -1;
    for (int i = 0; i < jj_2_rtns.length; i++) jj_2_rtns[i] = new JJCalls();
  }

  /** Constructor with generated Token Manager. */
  public JSGFParser(JSGFParserTokenManager tm) {
    token_source = tm;
    token = new Token();
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 26; i++) jj_la1[i] = -1;
    for (int i = 0; i < jj_2_rtns.length; i++) jj_2_rtns[i] = new JJCalls();
  }

  /** Reinitialise. */
  public void ReInit(JSGFParserTokenManager tm) {
    token_source = tm;
    token = new Token();
    jj_ntk = -1;
    jj_gen = 0;
    for (int i = 0; i < 26; i++) jj_la1[i] = -1;
    for (int i = 0; i < jj_2_rtns.length; i++) jj_2_rtns[i] = new JJCalls();
  }

  private Token jj_consume_token(int kind) throws ParseException {
    Token oldToken;
    if ((oldToken = token).next != null) token = token.next;
    else token = token.next = token_source.getNextToken();
    jj_ntk = -1;
    if (token.kind == kind) {
      jj_gen++;
      if (++jj_gc > 100) {
        jj_gc = 0;
        for (int i = 0; i < jj_2_rtns.length; i++) {
          JJCalls c = jj_2_rtns[i];
          while (c != null) {
            if (c.gen < jj_gen) c.first = null;
            c = c.next;
          }
        }
      }
      return token;
    }
    token = oldToken;
    jj_kind = kind;
    throw generateParseException();
  }

  static private final class LookaheadSuccess extends java.lang.Error { }
  final private LookaheadSuccess jj_ls = new LookaheadSuccess();
  private boolean jj_scan_token(int kind) {
    if (jj_scanpos == jj_lastpos) {
      jj_la--;
      if (jj_scanpos.next == null) {
        jj_lastpos = jj_scanpos = jj_scanpos.next = token_source.getNextToken();
      } else {
        jj_lastpos = jj_scanpos = jj_scanpos.next;
      }
    } else {
      jj_scanpos = jj_scanpos.next;
    }
    if (jj_rescan) {
      int i = 0; Token tok = token;
      while (tok != null && tok != jj_scanpos) { i++; tok = tok.next; }
      if (tok != null) jj_add_error_token(kind, i);
    }
    if (jj_scanpos.kind != kind) return true;
    if (jj_la == 0 && jj_scanpos == jj_lastpos) throw jj_ls;
    return false;
  }


/** Get the next Token. */
  final public Token getNextToken() {
    if (token.next != null) token = token.next;
    else token = token.next = token_source.getNextToken();
    jj_ntk = -1;
    jj_gen++;
    return token;
  }

/** Get the specific Token. */
  final public Token getToken(int index) {
    Token t = token;
    for (int i = 0; i < index; i++) {
      if (t.next != null) t = t.next;
      else t = t.next = token_source.getNextToken();
    }
    return t;
  }

  private int jj_ntk() {
    if ((jj_nt=token.next) == null)
      return (jj_ntk = (token.next=token_source.getNextToken()).kind);
    else
      return (jj_ntk = jj_nt.kind);
  }

  private java.util.List<int[]> jj_expentries = new java.util.ArrayList<int[]>();
  private int[] jj_expentry;
  private int jj_kind = -1;
  private int[] jj_lasttokens = new int[100];
  private int jj_endpos;

  private void jj_add_error_token(int kind, int pos) {
    if (pos >= 100) return;
    if (pos == jj_endpos + 1) {
      jj_lasttokens[jj_endpos++] = kind;
    } else if (jj_endpos != 0) {
      jj_expentry = new int[jj_endpos];
      for (int i = 0; i < jj_endpos; i++) {
        jj_expentry[i] = jj_lasttokens[i];
      }
      jj_entries_loop: for (java.util.Iterator<?> it = jj_expentries.iterator(); it.hasNext();) {
        int[] oldentry = (int[])(it.next());
        if (oldentry.length == jj_expentry.length) {
          for (int i = 0; i < jj_expentry.length; i++) {
            if (oldentry[i] != jj_expentry[i]) {
              continue jj_entries_loop;
            }
          }
          jj_expentries.add(jj_expentry);
          break jj_entries_loop;
        }
      }
      if (pos != 0) jj_lasttokens[(jj_endpos = pos) - 1] = kind;
    }
  }

  /** Generate ParseException. */
  public ParseException generateParseException() {
    jj_expentries.clear();
    boolean[] la1tokens = new boolean[40];
    if (jj_kind >= 0) {
      la1tokens[jj_kind] = true;
      jj_kind = -1;
    }
    for (int i = 0; i < 26; i++) {
      if (jj_la1[i] == jj_gen) {
        for (int j = 0; j < 32; j++) {
          if ((jj_la1_0[i] & (1<<j)) != 0) {
            la1tokens[j] = true;
          }
          if ((jj_la1_1[i] & (1<<j)) != 0) {
            la1tokens[32+j] = true;
          }
        }
      }
    }
    for (int i = 0; i < 40; i++) {
      if (la1tokens[i]) {
        jj_expentry = new int[1];
        jj_expentry[0] = i;
        jj_expentries.add(jj_expentry);
      }
    }
    jj_endpos = 0;
    jj_rescan_token();
    jj_add_error_token(0, 0);
    int[][] exptokseq = new int[jj_expentries.size()][];
    for (int i = 0; i < jj_expentries.size(); i++) {
      exptokseq[i] = jj_expentries.get(i);
    }
    return new ParseException(token, exptokseq, tokenImage);
  }

  /** Enable tracing. */
  final public void enable_tracing() {
  }

  /** Disable tracing. */
  final public void disable_tracing() {
  }

  private void jj_rescan_token() {
    jj_rescan = true;
    for (int i = 0; i < 1; i++) {
    try {
      JJCalls p = jj_2_rtns[i];
      do {
        if (p.gen > jj_gen) {
          jj_la = p.arg; jj_lastpos = jj_scanpos = p.first;
          switch (i) {
            case 0: jj_3_1(); break;
          }
        }
        p = p.next;
      } while (p != null);
      } catch(LookaheadSuccess ls) { }
    }
    jj_rescan = false;
  }

  private void jj_save(int index, int xla) {
    JJCalls p = jj_2_rtns[index];
    while (p.gen > jj_gen) {
      if (p.next == null) { p = p.next = new JJCalls(); break; }
      p = p.next;
    }
    p.gen = jj_gen + xla - jj_la; p.first = token; p.arg = xla;
  }

  static final class JJCalls {
    int gen;
    Token first;
    int arg;
    JJCalls next;
  }

}
