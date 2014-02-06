package edu.cmu.sphinx.demo.speakerid;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import edu.cmu.sphinx.speakerid.*;

public class SpeakerIdentificationDemo {

    /**
     * @param speakers
     *            An array of clusters for which it is needed to be printed the
     *            speakers intervals
     * @throws IOException
     */
    public static void printSpeakerIntervals(ArrayList<SpeakerCluster> speakers, String fileName) throws IOException {
        String ofName = fileName.substring(0, fileName.indexOf('.')) + ".seg";
        FileWriter fr = new FileWriter(ofName);
        int spk = 0;
        for (SpeakerCluster sc : speakers) {
            spk++;
            ArrayList<Integer> t = sc.getSpeakerIntervals();
            for (int j = 0; j < t.size() / 2; j++)
                fr.write(fileName + " " + 1 + " " + t.get(2 * j) / 10 + " "
                    + t.get(2 * j + 1) / 10 + " U U U S" + spk + '\n');
        }
        fr.close();
    }

    public static void main(String[] args) throws IOException {
        String inputFile = "src/apps/edu/cmu/sphinx/demo/speakerid/test.wav";
        SpeakerIdentification sd = new SpeakerIdentification();
        ArrayList<SpeakerCluster> clusters = sd.cluster(inputFile);
        printSpeakerIntervals(clusters, inputFile);
    }
}
