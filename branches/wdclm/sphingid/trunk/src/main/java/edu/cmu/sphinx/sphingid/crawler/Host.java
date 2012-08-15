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
public class Host {
	private String url;
	private RobotSettings robotSettings;

	public Host(String url) throws IOException,
			CrawlDelayException, NoPageAllowedException {
		URL temp = new URL(url);
		this.robotSettings = RobotsParser.getInstance().parse(
				new URL(temp.getProtocol() + "://" + temp.getHost() //$NON-NLS-1$
						+ "/robots.txt")); //$NON-NLS-1$
		this.url = url;
	}

	private Host() {
		super();
	}

	public long getCrawlDelay() {
		return this.robotSettings.getCrawlDelay();
	}

	public RobotSettings getRobotSettings() {
		return this.robotSettings;
	}

	public String getUrl() {
		return this.url;
	}
}
