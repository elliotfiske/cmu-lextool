package edu.cmu.sphinx.diarization;

import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;

public class SpeakerDiarizationDemo {
	/**
	 * 
	 * @param speakers
	 *            An array of clusters for which it is needed to be printed the
	 *            speakers intervals
	 * @throws IOException
	 */
	public static void printSpeakerIntervals(
			ArrayList<SpeakerCluster> speakers, String fileName)
			throws IOException {
		String ofName = fileName.substring(0, fileName.indexOf('.')) + ".seg";
		FileWriter fr = new FileWriter(ofName);
		for (int i = 0; i < speakers.size(); i++) {
			ArrayList<Integer> t = speakers.get(i).getSpeakerIntervals();
			for (int j = 0; j < t.size() / 2; j++)
				fr.write(fileName + " " + 1 + " " + t.get(2 * j) + " "
						+ t.get(2 * j + 1) + " U U U S" + i + '\n');
		}
		fr.close();
	}
	/**
	 * @param args
	 */
	public static void main(String[] args) throws IOException {
		String inputFile = "librivox_community_podcast_135.wav";
		SpeakerDiarization sd = new SpeakerDiarization();
		ArrayList<SpeakerCluster> clusters = sd.cluster(inputFile);
		printSpeakerIntervals(clusters, inputFile);
	}

}
