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
import java.util.GregorianCalendar;

import edu.cmu.sphinx.fst.decoder.Decoder;
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
        long start = GregorianCalendar.getInstance().getTimeInMillis();

        String model = args[0];
        String words = args[1];
        int best = Integer.parseInt(args[2]);

        Decoder d = new Decoder(model);
        String[] syms = d.getModelIsyms();

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
        int wordCount = 0;
        try {
            while ((strLine = br.readLine()) != null) {
                String[] tokens = strLine.split("  ");
                String in = tokens[0].toLowerCase();

                // convert it to ArrayList<String>
                ArrayList<String> entry = new ArrayList<String>(in.length());
                for (int i = 0; i < in.length(); i++) {
                    String ch = in.substring(i, i + 1);
                    if (Utils.getIndex(syms, ch) >= 0) {
                        entry.add(ch);
                    }
                }

                ArrayList<Path> res = d.phoneticize(entry, best);
                wordCount++;
                for (Path p : res) {
                    System.out.print(in.toUpperCase() + "\t");
                    System.out.print(Utils.round(p.getCost(), 4) + "\t");
                    int count = 0;
                    for (String str : p.getPath()) {
                        count++;
                        System.out.print(str);
                        if (count != p.getPath().size()) {
                            System.out.print(" ");
                        }
                    }
                    System.out.println();
                }
                if (res.size() == 0) {
                    // just print a new line
                    System.out.println();
                }
            }
        } catch (IOException e) {
            e.printStackTrace();
            System.exit(1);
        }
        long total = +(GregorianCalendar.getInstance().getTimeInMillis() - start);
        System.err.println("Total Time (s): " + total / 1000.);
        System.err.println("Time per Word (ms): "
                + Utils.round((float) 1. * total / wordCount, 1));
    }
}
