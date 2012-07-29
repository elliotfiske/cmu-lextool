/**
 * 
 */
package edu.cmu.sphinx.fst.decoder;

import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.Vector;

import edu.cmu.sphinx.fst.decoder.Decoder;
import edu.cmu.sphinx.fst.decoder.Path;
import edu.cmu.sphinx.fst.utils.Mapper;
import edu.cmu.sphinx.fst.utils.Utils;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Phoneticize {

	/**
	 * @param args[0] model's filename
	 * @param args[1] file containing the words to phonetize
	 * @param args[2] number of best paths to return
	 */
	public static void main(String[] args) {
    	String model = args[0];
		String words = args[1];
		int best = Integer.parseInt(args[2]);
//    	String model = "../data/20120728/models/6gram.fst.ser";
//		String words = "../data/20120728/test";
//		int best = 1;

		Decoder d = new Decoder(model);
		Mapper<Integer, String> syms = d.getIsyms();
		
		// Parse input
		FileInputStream fis = null;
		try {
			fis = new FileInputStream(words);
		} catch (FileNotFoundException e1) {
			e1.printStackTrace();
			System.exit(1);
		}
		
		DataInputStream dis = new DataInputStream(fis);			
		BufferedReader br = new BufferedReader(new InputStreamReader(dis));
		String strLine;
		String[] tokens;
		String in;
		Vector<String> entry;
		String ch;
		ArrayList<Path> res;
		Path p;
		try {
			while ((strLine = br.readLine()) != null) {
				tokens = strLine.split("  ");
				in = tokens[0]; 

				//convert it to Vector<String> 
				entry = new Vector<String>();
				for(int i=0; i< in.length(); i++) {
					ch = in.substring(i, i+1);
					if(syms.getKey(ch)!=null) {
						entry.add(ch);
					}
				}
		
				res = d.phoneticize(entry, best);
				System.out.print(in +"\t");
				for(Iterator<Path> it = res.iterator(); it.hasNext();) {
					p = it.next();
					System.out.print(Utils.round(p.getCost(), 4) + "\t");
					for(Iterator<String> itS = p.getPath().iterator(); itS.hasNext();) {
						ch = itS.next();
						System.out.print(ch);
						if(itS.hasNext()) {
							System.out.print(" ");
						}
					}
					System.out.println();
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(1);
		}
	}
}
