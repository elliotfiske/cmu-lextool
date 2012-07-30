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
        // String model = "../data/20120728/models/6gram.fst.ser";
        // String words = "../data/20120728/test";
        // int best = 1;

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
        try {
            while ((strLine = br.readLine()) != null) {
                String[] tokens = strLine.split("  ");
                String in = tokens[0];

                // convert it to Vector<String>
                Vector<String> entry = new Vector<String>();
                for (int i = 0; i < in.length(); i++) {
                    String ch = in.substring(i, i + 1);
                    if (syms.getKey(ch) != null) {
                        entry.add(ch);
                    }
                }

                ArrayList<Path> res = d.phoneticize(entry, best);
                System.out.print(in + "\t");
                for (Path p : res) {
                    System.out.print(Utils.round(p.getCost(), 4) + "\t");
                    for (String str : p.getPath()) {
                        System.out.print(str);
                        if (!str.equals(p.getPath().get(p.getPath().size() - 1))) {
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
