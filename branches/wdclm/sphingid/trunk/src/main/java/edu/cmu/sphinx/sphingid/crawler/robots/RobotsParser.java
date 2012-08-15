package edu.cmu.sphinx.sphingid.crawler.robots;

import java.io.BufferedReader;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.MalformedURLException;
import java.net.URL;
import java.nio.charset.Charset;
import java.util.ArrayList;
import java.util.StringTokenizer;
import java.util.regex.Pattern;

import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings.NoPageAllowedException;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 * 
 */
public class RobotsParser {
	public class CrawlDelayException extends Exception {
		private String message;

		/**
		 * 
		 */
		private static final long serialVersionUID = -2916871771567770870L;

		public CrawlDelayException(String message) {
			this.message = message;
		}

		@Override
		public String getMessage() {
			return this.message;
		}
	}

	private static final Logger logger = LoggerFactory
			.getLogger(RobotsParser.class);
	private static RobotsParser instance = new RobotsParser();

	public static RobotsParser getInstance() {
		return instance;
	}

	private String crawlerName;

	private long defaultCrawlDelay, maxCrawlDelay;

	private RobotsParser() {
		this.crawlerName = "Sphingid"; //$NON-NLS-1$
		this.defaultCrawlDelay = 5000L;
		this.maxCrawlDelay = 30000L;
	}

	public void initialize(String crawlerName, long defaultCrawlDelay,
			long maxCrawlDelay) {
		this.crawlerName = crawlerName;
		this.defaultCrawlDelay = defaultCrawlDelay;
		this.maxCrawlDelay = maxCrawlDelay;
	}

	public RobotSettings parse(URL robotsUrl) throws CrawlDelayException,
			NoPageAllowedException, NumberFormatException,
			MalformedURLException, IOException {

		ArrayList<RobotRule> ruleList = new ArrayList<RobotRule>();
		ArrayList<String> sitemapList = new ArrayList<String>();
		long crawlDelay = this.defaultCrawlDelay;

		/*
		 * Read the contents of the URL
		 */
		BufferedReader br = null;
		try {
			InputStream is = robotsUrl.openStream();

			br = new BufferedReader(new InputStreamReader(is,
					Charset.forName("utf-8"))); //$NON-NLS-1$
		} catch (FileNotFoundException e) {
			logger.warn(
					"robots.txt could not be found at {}. Assuming everything is okay and using default crawl delay.", //$NON-NLS-1$
					robotsUrl.toString());
			return new RobotSettings(new ArrayList<RobotRule>(),
					new ArrayList<String>(), this.defaultCrawlDelay);
		}

		logger.info("robots.txt parsed for {}.", robotsUrl.getHost()); //$NON-NLS-1$

		/*
		 * Begin parsing
		 */

		boolean areRulesSpeciallyForUs = false;
		String line;
		while (br.ready() && ((line = br.readLine()) != null)) {
			StringTokenizer tokenizer = new StringTokenizer(line);
			if (tokenizer.countTokens() < 2)
				continue;

			String rule = tokenizer.nextToken().toLowerCase();

			if (rule.equals("user-agent:")) { //$NON-NLS-1$

				String agent = tokenizer.nextToken();

				if (!agent.equals(this.crawlerName) && !agent.equals("*")) { //$NON-NLS-1$
					/*
					 * Irrelevant entry.
					 */
					continue;
				} else if (agent.equals("*") && areRulesSpeciallyForUs) { //$NON-NLS-1$
					/*
					 * We have special rules, we don't care for "*"
					 */
					continue;
				} else if (agent.equals(this.crawlerName)
						&& !areRulesSpeciallyForUs) {
					/*
					 * We have special rules incoming, throw away the ones that
					 * were for "*"
					 */
					areRulesSpeciallyForUs = true;
					ruleList = new ArrayList<RobotRule>();
					crawlDelay = this.defaultCrawlDelay;
				}

				while (br.ready() && ((line = br.readLine()) != null)) {
					tokenizer = new StringTokenizer(line);
					if (tokenizer.countTokens() < 2)
						continue;

					rule = tokenizer.nextToken().toLowerCase();

					if (rule.equals("disallow:")) { //$NON-NLS-1$
						String expressionString = tokenizer.nextToken();
						expressionString = expressionString.replaceAll(
								Pattern.quote("*"), ".*"); //$NON-NLS-1$ //$NON-NLS-2$

						expressionString += "\\b"; //$NON-NLS-1$

						ruleList.add(new RobotRule(RobotRule.Rule.DISALLOW,
								expressionString));
					} else if (rule.equals("allow:")) { //$NON-NLS-1$
						String expressionString = tokenizer.nextToken();
						expressionString = expressionString.replaceAll(
								Pattern.quote("*"), ".*"); //$NON-NLS-1$ //$NON-NLS-2$

						expressionString += "\\b"; //$NON-NLS-1$
						ruleList.add(new RobotRule(RobotRule.Rule.ALLOW,
								expressionString));
					} else if (rule.equals("crawl-delay:")) { //$NON-NLS-1$
						/*
						 * Take the higher crawl delay value for politeness
						 */
						long newCrawlDelay = (long) (Float.parseFloat(tokenizer
								.nextToken()) * 1000.0F);
						if (crawlDelay < newCrawlDelay)
							crawlDelay = newCrawlDelay;
					} else
						break;
				}
			}
			if (rule.equals("sitemap:")) { //$NON-NLS-1$
				String sitemap = tokenizer.nextToken();
				sitemapList.add(sitemap);
			}
		}

		br.close();

		if (crawlDelay > this.maxCrawlDelay) {
			throw new CrawlDelayException(
					String.format(
							"Robot settings for %s is %f second(s), which is higher than the maximum setting %f second(s) in configuration file. Skipping this host.", //$NON-NLS-1$
							robotsUrl.getHost(), crawlDelay / 1000.0F,
							this.maxCrawlDelay / 1000.0F));
		}

		RobotSettings robotSettings = new RobotSettings(ruleList, sitemapList,
				crawlDelay);

		return robotSettings;
	}
}
