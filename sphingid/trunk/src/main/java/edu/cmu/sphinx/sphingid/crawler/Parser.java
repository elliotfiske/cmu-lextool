/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.net.MalformedURLException;
import java.net.URL;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Collections;

import org.jsoup.Jsoup;
import org.jsoup.nodes.Document;
import org.jsoup.nodes.Element;

import de.l3s.boilerpipe.BoilerpipeProcessingException;
import de.l3s.boilerpipe.extractors.ArticleExtractor;
import de.l3s.boilerpipe.extractors.KeepEverythingExtractor;

/**
 * Parser class that can extract text and links from a web page using Boilerpipe
 * and JSoup libraries or return hashes of them.
 * <p>
 * Hashing algorithm is SHA-256.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class Parser {
	public static ArrayList<String> extractLinks(URL baseUrl, String data)
			throws MalformedURLException {
		Document document = Jsoup.parse(data);

		ArrayList<String> links = new ArrayList<String>();

		for (Element link : document.select("a[href]")) { //$NON-NLS-1$
			String href = link.attr("abs:href"); //$NON-NLS-1$
			if (href.isEmpty()) {
				href = link.attr("href"); //$NON-NLS-1$
			}

			String url = generateUrl(baseUrl, href);
			if (url != null) {
				links.add(url);
			}
		}
		removeDuplicates(links);

		return links;
	}

	public static String generateUrl(URL baseUrl, String href)
			throws MalformedURLException {
		URL url = null;
		if (href.startsWith("http")) { //$NON-NLS-1$
			url = new URL(href);
		} else if (href.startsWith("?")) { //$NON-NLS-1$
			url = new URL(baseUrl + "/" + href); //$NON-NLS-1$
		} else if (!href.contains("://") && !href.startsWith("jav")) { //$NON-NLS-1$ //$NON-NLS-2$
			url = new URL(baseUrl, href);
		} else {
			return null;
		}

		if (url.getHost().contains(baseUrl.getHost())) {
			String urlString = url.toString();
			urlString = urlString.replaceAll("#.*", ""); //$NON-NLS-1$ //$NON-NLS-2$
			return urlString;
		}

		return null;
	}

	public static Parser getInstance() {
		return instance;
	}

	public static void removeDuplicates(ArrayList<String> list) {
		/*
		 * Remove duplicates nlogn + n (assuming O(1) deletion, which is not the
		 * case)
		 */
		Collections.sort(list);
		for (int i = list.size() - 1; i > 0; i--) {
			if (list.get(i).equals(list.get(i - 1))) {
				list.remove(i);
			}
		}
	}

	private MessageDigest messageDigest;

	private ArticleExtractor articleExtractor;

	private KeepEverythingExtractor textExtractor;

	private static Parser instance = new Parser();

	private Parser() {
		try {
			this.messageDigest = MessageDigest.getInstance("SHA-256"); //$NON-NLS-1$
			this.articleExtractor = ArticleExtractor.getInstance();
			this.textExtractor = KeepEverythingExtractor.INSTANCE;
		} catch (NoSuchAlgorithmException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
	}

	public String extractText(String htmlDocument)
			throws BoilerpipeProcessingException {
		return this.textExtractor.getText(htmlDocument);
	}

	public String hash256(String string) {
		byte[] array = null;
		synchronized (this) {
			this.messageDigest.reset();
			array = this.messageDigest.digest(string.getBytes());
		}
		StringBuffer sb = new StringBuffer();
		for (int i = 0; i < array.length; i++)
			sb.append(Integer.toHexString(array[i] & 0xFF));

		return sb.toString();
	}

	public long hash64(String string) {
		byte[] array = null;
		synchronized (this) {
			this.messageDigest.reset();
			array = this.messageDigest.digest(string.getBytes());
		}
		return ((long) (array[0] & 0xff) << 56)
				| ((long) (array[1] & 0xff) << 48)
				| ((long) (array[2] & 0xff) << 40)
				| ((long) (array[3] & 0xff) << 32)
				| ((long) (array[4] & 0xff) << 24)
				| ((long) (array[5] & 0xff) << 16)
				| ((long) (array[6] & 0xff) << 8) | (array[7] & 0xff);
	}

	public String parseArticle(String htmlDocument)
			throws BoilerpipeProcessingException {
		return this.articleExtractor.getText(htmlDocument);
	}
}
