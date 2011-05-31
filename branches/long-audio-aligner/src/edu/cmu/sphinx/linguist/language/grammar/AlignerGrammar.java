package edu.cmu.sphinx.linguist.language.grammar;

import java.util.List;
import java.io.IOException;
import java.io.StringReader;
import java.util.ArrayList;

import edu.cmu.sphinx.linguist.dictionary.Dictionary;

import edu.cmu.sphinx.util.ExtendedStreamTokenizer;
import edu.cmu.sphinx.util.LogMath;
import edu.cmu.sphinx.util.props.PropertyException;
import edu.cmu.sphinx.util.props.PropertySheet;
import edu.cmu.sphinx.util.props.S4Component;

public class AlignerGrammar extends Grammar 
{
	@S4Component(type=LogMath.class)
	public final static String PROP_LOG_MATH = "logMath";
	private LogMath logMath;			
	private int start;
	
	protected GrammarNode finalNode;
	private final List<String> tokens = new ArrayList<String>();
	
	
	public AlignerGrammar(final String text, final LogMath logMath, final boolean showGrammar, final boolean optimizeGrammar,
            final boolean addSilenceWords, final boolean addFillerWords, final Dictionary dictionary) 
	{
        super(showGrammar, optimizeGrammar, addSilenceWords, addFillerWords, dictionary);
        this.logMath = logMath;
        setText(text);
    }
	public AlignerGrammar()
	{
		
	}
	/*
	 * Reads Text and converts it into a list of tokens
	 */
	public void setText(String text) {
        String word;
        try 
        {
            final ExtendedStreamTokenizer tok = new ExtendedStreamTokenizer(new StringReader(text), true);
            
            tokens.clear();
            while (!tok.isEOF()) 
            {
                while ((word = tok.getString()) != null) 
                {
                    word = word.toLowerCase();
                    tokens.add(word);
                }
            }
        } 
        catch (Exception e) 
        {
            e.printStackTrace();
        }
    }
	

	@Override
    public void newProperties(PropertySheet ps) throws PropertyException {
        super.newProperties(ps);
        logMath = (LogMath) ps.getComponent(PROP_LOG_MATH);
    }
	
	
	@Override
	protected GrammarNode createGrammar() throws IOException 
	{
		initialNode = createGrammarNode(Dictionary.SILENCE_SPELLING);
        finalNode = createGrammarNode(Dictionary.SILENCE_SPELLING);
        finalNode.setFinalNode(true);
        final GrammarNode branchNode = createGrammarNode(false);
        
        final List<GrammarNode> wordGrammarNodes = new ArrayList<GrammarNode>();
        final int end = tokens.size();
        
        for (final String word : tokens.subList(start, end)) 
        {
            // System.out.println ("Creating grammar from " + word);
            final GrammarNode wordNode = createGrammarNode(word.toLowerCase());
            wordGrammarNodes.add(wordNode);
            //System.out.println(word);
        }
        
        // now connect all the GrammarNodes together
        initialNode.add(branchNode, LogMath.getLogOne());
        

        for (int i = 0; i < wordGrammarNodes.size(); i++) 
        {
            final GrammarNode wordNode = wordGrammarNodes.get(i);
            
            float branchScore=logMath.linearToLog(1.0);            
            branchNode.add(wordNode, branchScore);           
            wordNode.add(finalNode, logMath.linearToLog(1.0/((wordGrammarNodes.size()-i)*(wordGrammarNodes.size()-i))));

            // add connections to close words
            for (int j = i + 1; j < i+10; j++) 
            {
                if (0 <= j && j<wordGrammarNodes.size()) 
                {
                    final GrammarNode neighbour = wordGrammarNodes.get(j);
                    wordNode.add(neighbour, logMath.linearToLog(1.0/((j-i)*(j-i))));
                }
            }
        }
        
        return initialNode;
	}

}