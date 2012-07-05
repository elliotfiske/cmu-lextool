package edu.cmu.sphinx.sphingid;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;

public class KMeans {
	public static void IRSTLMCluster(ArrayList<TextDocument> documents,
			int numClusters) {
		IRSTLMCluster(documents, numClusters, 3, "wb", false);
	}

	@SuppressWarnings("unchecked")
	public static void IRSTLMCluster(ArrayList<TextDocument> documents,
			int numClusters, int ngramCount, String smoothing, boolean backoff) {
		long startTime = System.currentTimeMillis();
		int numTextDocuments = documents.size();
		ArrayList<ArrayList<TextDocument>> clusters = new ArrayList<ArrayList<TextDocument>>(
				numClusters);
		double[] perplexities = new double[numClusters];

		for (int i = 0; i < numClusters; i++) {
			clusters.add(new ArrayList<TextDocument>((int) (Math
					.floor(numTextDocuments / numClusters * 1.5))));
		}

		// Initialize clusters by assigning randomly chosen numCluster documents
		// to a unique cluster each
		Collections.shuffle(documents);
		for (int i = 0; i < numClusters; i++) {
			clusters.get(i).add(documents.get(i));
			new File("lms/" + i + ".blm").delete();
			IRSTLMWrapper.buildLM(documents.get(i).getFile(), new File("lms/"
					+ i + ".blm"), ngramCount, smoothing, backoff);
		}

		// Calculate initial perplexities and assign clusters
		long endTime = System.currentTimeMillis();
		System.out.println("Starting initial cluster assignment of "
				+ documents.size() + " documents at " + (endTime - startTime)
				+ " ms");

		for (int i = numClusters; i < numTextDocuments; i++) {

			// Find the best cluster
			double bestScore = Double.MAX_VALUE;
			int bestCluster = -1;
			for (int j = 0; j < numClusters; j++) {
				double perplexity = IRSTLMWrapper.computePerplexity(new File(
						"lms/" + j + ".blm"), documents.get(i).getFile());
				if (perplexity < bestScore) {
					bestScore = perplexity;
					bestCluster = j;
				}
			}

			// Assign to the best cluster
			clusters.get(bestCluster).add(documents.get(i));

			// Display stats
			if (i % 50 == 0) {
				endTime = System.currentTimeMillis();
				System.out.println("At document #" + i + "... Time elapsed: "
						+ (endTime - startTime) + " ms");
			}

		}

		// Rebuild LM for the cluster
		for (int i = 0; i < numClusters; i++) {
			new File("lms/" + i + ".blm").delete();
			File combined = FileUtils.combineTextDocumentsToFile(
					clusters.get(i), "/tmp/");

			IRSTLMWrapper.buildLM(combined, new File("lms/" + i + ".blm"),
					ngramCount, smoothing, backoff);

			combined.delete();

		}
		endTime = System.currentTimeMillis();
		System.out.println("Initial cluster assignment of " + documents.size()
				+ " documents finished at " + (endTime - startTime) + " ms");

		System.out.println("Initial clusters:");
		for (int i = 0; i < numClusters; i++) {
			System.out.println("Cluster " + i + " (" + clusters.get(i).size()
					+ ") :");
			for (TextDocument document : clusters.get(i)) {
				System.out.print(document.getName() + ", ");
			}
			System.out.println("\n");
		}

		// Will be used for checking if there is a change or not in clusters
		ArrayList<ArrayList<TextDocument>> oldClusters;

		endTime = System.currentTimeMillis();
		System.out.println("Starting clustering at " + (endTime - startTime)
				+ " ms");

		// How many iterations of k-Means?
		int iterationTimes = 1;

		do {
			// Create a new cluster list by copying.
			// Can't clone clusters, it's shallow copy.
			oldClusters = new ArrayList<ArrayList<TextDocument>>();
			for (int i = 0; i < numClusters; i++) {
				oldClusters.add((ArrayList<TextDocument>) clusters.get(i)
						.clone());
				perplexities[i] = 0;
			}

			for (int i = 0; i < numTextDocuments; i++) {
				for (int j = 0; j < numClusters; j++) {
					// Removing document from its cluster
					if (clusters.get(j).contains(documents.get(i))) {

						clusters.get(j).remove(documents.get(i));

						new File("lms/" + j + ".blm").delete();
						File combined = FileUtils.combineTextDocumentsToFile(
								clusters.get(j), "/tmp/");

						// Build new LM for the cluster without the document
						IRSTLMWrapper.buildLM(combined, new File("lms/" + j
								+ ".blm"), ngramCount, smoothing, backoff);

						combined.delete();

						break;
					}
				}

				// Find best
				double bestScore = Double.MAX_VALUE;
				int bestCluster = -1;

				for (int j = 0; j < numClusters; j++) {
					double perplexity = IRSTLMWrapper.computePerplexity(
							new File("lms/" + j + ".blm"), documents.get(i)
									.getFile());
					if (perplexity < bestScore) {
						bestScore = perplexity;
						bestCluster = j;
					}
				}

				// Assign to the best cluster
				clusters.get(bestCluster).add(documents.get(i));
				new File("lms/" + bestCluster + ".blm").delete();
				File combined = FileUtils.combineTextDocumentsToFile(
						clusters.get(bestCluster), "/tmp/");

				// Add document perplexity to total perplexity for the cluster
				perplexities[bestCluster] += bestScore;

				// Display stats
				if (i % 50 == 0) {
					endTime = System.currentTimeMillis();
					System.out.println("At document #" + i + " of iteration "
							+ iterationTimes + "... Time elapsed: "
							+ (endTime - startTime) + " ms");
				}
			}
			// Rebuild LM for the cluster
			for (int i = 0; i < numClusters; i++) {
				new File("lms/" + i + ".blm").delete();
				File combined = FileUtils.combineTextDocumentsToFile(
						clusters.get(i), "/tmp/");

				IRSTLMWrapper.buildLM(combined, new File("lms/" + i + ".blm"),
						ngramCount, smoothing, backoff);

				combined.delete();

			}

			// Display iteration stats
			endTime = System.currentTimeMillis();
			System.out.println("Cluster assignment of " + documents.size()
					+ " documents for iteration " + iterationTimes
					+ " finished at " + (endTime - startTime) + " ms");

			System.out.println("Clusters at iteration " + iterationTimes + ":");
			for (int i = 0; i < numClusters; i++) {
				System.out.println("Cluster " + i + " ("
						+ clusters.get(i).size()
						+ ") has a total perplexity of " + perplexities[i]
						+ ". The documents belonging to the cluster are:");
				for (TextDocument document : clusters.get(i)) {
					System.out.print(document.getName() + ", ");
				}
				System.out.println("\n");
			}

			iterationTimes++;
		} while (!oldClusters.equals(clusters));
	}

}
