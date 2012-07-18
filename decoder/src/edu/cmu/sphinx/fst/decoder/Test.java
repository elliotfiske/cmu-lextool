/**
 * 
 */
package edu.cmu.sphinx.fst.decoder;

import java.util.ArrayList;
import java.util.Vector;

import edu.cmu.sphinx.fst.decoder.Decoder;
import edu.cmu.sphinx.fst.decoder.Path;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Test {

	/**
	 * @param args[0] model's filename
	 * @param args[1] word to phoneticize
	 * @param args[2] number of best paths to return
	 */
	public static void main(String[] args) {
		String model = args[0];
		Decoder d = new Decoder(model);
		
		String in = args[1];
		int best = Integer.parseInt(args[2]);
		
		//convert it to Vector<String> 
		Vector<String> entry = new Vector<String>();
		for(int i=0; i< in.length(); i++) {
			String ch = Character.toString(in.charAt(i));
			entry.add(ch);
		}
		
		ArrayList<Path<Double>> res = d.phoneticize(entry, best);

		for(int i=0;i<res.size();i++) {
			Path<Double> p = res.get(i);
			System.out.println(p);
		}
	}
}
