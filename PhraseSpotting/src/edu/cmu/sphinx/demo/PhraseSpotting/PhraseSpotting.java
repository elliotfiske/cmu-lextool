/*
 * Copyright 1999-2002 Carnegie Mellon University.  
 * Portions Copyright 2002 Sun Microsystems, Inc.  
 * Portions Copyright 2002 Mitsubishi Electric Research Laboratories.
 * All Rights Reserved.  Use is subject to license terms.
 * 
 * See the file "license.terms" for information on usage and
 * redistribution of this file, and for a DISCLAIMER OF ALL 
 * WARRANTIES.
 *
 */

package edu.cmu.sphinx.demo.PhraseSpotting;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.Iterator;
import java.util.List;


import edu.cmu.sphinx.PhraseSpotter.PhraseSpotter;
import edu.cmu.sphinx.PhraseSpotter.Result;
import edu.cmu.sphinx.PhraseSpotter.SimplePhraseSpotter.SimplePhraseSpotter;
import edu.cmu.sphinx.frontend.util.AudioFileDataSource;
import edu.cmu.sphinx.linguist.PhraseSpottingFlatLinguist.PhraseSpottingFlatLinguist;
import edu.cmu.sphinx.linguist.language.grammar.NoSkipGrammar;
import edu.cmu.sphinx.util.props.ConfigurationManager;

public class PhraseSpotting {
	public static void main(String Args[]) throws MalformedURLException{
		
		SimplePhraseSpotter spotter = new SimplePhraseSpotter("./src/config.xml");
		String phrase = "buck was";
		spotter.setPhrase(phrase);
		spotter.setAudioDataSource(new URL("file:./resource/wav/call_of_the_wild_chapter_01.wav"));
		spotter.allocate();
		spotter.startSpotting();
		List<Result> result = spotter.getTimedResult();
		Iterator<Result> iter = result.iterator();
		System.out.println("Times when \"" + phrase + "\" was spotted");
		while(iter.hasNext()){
			Result data = iter.next();
			System.out.println("(" + data.getStartTime() + "," + data.getEndTime() + ")");
		}
	}
}
