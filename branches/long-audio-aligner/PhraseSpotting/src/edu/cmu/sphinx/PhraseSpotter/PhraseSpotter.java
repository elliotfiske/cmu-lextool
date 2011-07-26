package edu.cmu.sphinx.PhraseSpotter;

import java.util.List;
import edu.cmu.sphinx.util.props.Configurable;

public interface PhraseSpotter extends Configurable {
	
	/**
	 * Hopefully there will be things here that will need configuration
	 */
	public void allocate();
	
	public void deallocate();
	
	public void setPhrase(String phrase);
	
	public void startSpotting();
	
	public List<Result> getTimedResult();

}
