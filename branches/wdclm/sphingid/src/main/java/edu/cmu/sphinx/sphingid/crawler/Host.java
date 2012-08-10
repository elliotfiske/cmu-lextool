package edu.cmu.sphinx.sphingid.crawler;

import java.io.IOException;
import java.io.Serializable;
import java.net.MalformedURLException;
import java.net.URL;

import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings.NoPageAllowedException;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser.CrawlDelayException;

/**
 * A class that represents a host on the web.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 */
public class Host implements Serializable {
	private static final long serialVersionUID = 3087728909594575128L;
	private URL url;
	private RobotSettings robotSettings;

	public Host(URL url) throws MalformedURLException, IOException,
			CrawlDelayException, NoPageAllowedException {
		this.robotSettings = RobotsParser.getInstance().parse(
				new URL(url.getProtocol() + "://" + url.getHost() //$NON-NLS-1$
						+ "/robots.txt")); //$NON-NLS-1$
		this.url = url;
	}

	public long getCrawlDelay() {
		return this.robotSettings.getCrawlDelay();
	}

	public RobotSettings getRobotSettings() {
		return this.robotSettings;
	}

	public URL getUrl() {
		return this.url;
	}
}
