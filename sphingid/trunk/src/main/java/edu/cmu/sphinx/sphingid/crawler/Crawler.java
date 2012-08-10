/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import java.io.File;
import java.io.FilenameFilter;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;
import java.util.List;
import java.util.regex.Pattern;

import org.apache.commons.configuration.HierarchicalConfiguration;
import org.apache.commons.configuration.SubnodeConfiguration;
import org.apache.commons.configuration.XMLConfiguration;
import org.apache.commons.lang.exception.ExceptionUtils;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

import edu.cmu.sphinx.sphingid.commons.FileUtils;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule.Rule;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings.NoPageAllowedException;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser.CrawlDelayException;

/**
 * Crawler class that reads a configuration file and creates crawler threads
 * that run {@link CrawlerRunnable}.
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public final class Crawler {
	private static final Logger logger = LoggerFactory.getLogger(Crawler.class);

	/**
	 * Reads the given configuration file and starts a thread that runs a
	 * {@link CrawlerRunnable} for each seed URL.
	 * 
	 * @param crawlerConfiguration
	 *            configuration to be used while crawling
	 */
	public Crawler(XMLConfiguration crawlerConfiguration,
			boolean isIncrementalCrawl) {
		if (isIncrementalCrawl == false) {
			/*
			 * Read crawler name
			 */
			String crawlerName = crawlerConfiguration.getString("name"); //$NON-NLS-1$

			/*
			 * Read time settings
			 */
			long defaultCrawlDelay = (long) (crawlerConfiguration
					.getFloat("timeSettings.defaultCrawlDelay") * 1000F); //$NON-NLS-1$
			long maxCrawlDelay = (long) (crawlerConfiguration
					.getFloat("timeSettings.maxCrawlDelay") * 1000F); //$NON-NLS-1$
			long defaultRecrawlInterval = (long) (crawlerConfiguration
					.getFloat("timeSettings.defaultRecrawlInterval") * 1000F); //$NON-NLS-1$
			long minimumRecrawlInterval = (long) (crawlerConfiguration
					.getFloat("timeSettings.minimumRecrawlInterval") * 1000F); //$NON-NLS-1$
			float recrawlTimeMultiplier = crawlerConfiguration
					.getFloat("timeSettings.recrawlMultiplier"); //$NON-NLS-1$

			/*
			 * Create fetch and recrawl schedulers
			 */
			RecrawlScheduler recrawlScheduler = new SimpleRecrawlScheduler(
					defaultRecrawlInterval, minimumRecrawlInterval,
					recrawlTimeMultiplier);
			FetchScheduler fetchScheduler = new EarliestDeadlineFirstScheduler();

			/*
			 * Set robots parser settings
			 */
			RobotsParser robotsParser = RobotsParser.getInstance();
			robotsParser.initialize(crawlerName, defaultCrawlDelay,
					maxCrawlDelay);

			/*
			 * Read general URL filters
			 */
			SubnodeConfiguration urlFilterConfigurations = crawlerConfiguration
					.configurationAt("urlFilters"); //$NON-NLS-1$
			ArrayList<RobotRule> generalUrlFilters = readUrlFilters(urlFilterConfigurations);

			/*
			 * Read seed URLs and their configurations, create hosts
			 */
			ArrayList<Host> hosts = new ArrayList<Host>();
			List<HierarchicalConfiguration> seedUrls = crawlerConfiguration
					.configurationsAt("seedUrls.seedUrl"); //$NON-NLS-1$

			for (HierarchicalConfiguration seedUrl : seedUrls) {
				URL url = null;
				String urlString = seedUrl.getString("url"); //$NON-NLS-1$
				try {
					url = new URL(urlString);
				} catch (MalformedURLException e) {
					logger.warn(
							Messages.getString("Crawler.0"), //$NON-NLS-1$
							urlString);
					logger.debug(ExceptionUtils.getStackTrace(e));
					continue;
				}
				SubnodeConfiguration customUrlFilterConfiguration = seedUrl
						.configurationAt("customUrlFilters"); //$NON-NLS-1$
				ArrayList<RobotRule> customUrlFilters = readUrlFilters(customUrlFilterConfiguration);
				Host host = null;

				try {
					host = new Host(url);
				} catch (MalformedURLException e) {
					logger.warn(
							Messages.getString("Crawler.12"), //$NON-NLS-1$
							url);
					logger.debug(ExceptionUtils.getStackTrace(e));
					continue;
				} catch (IOException e) {
					logger.warn(
							Messages.getString("Crawler.11"), //$NON-NLS-1$
							url);
					logger.debug(ExceptionUtils.getStackTrace(e));
					continue;
				} catch (CrawlDelayException e) {
					logger.warn(e.getMessage());
					continue;
				} catch (NoPageAllowedException e) {
					logger.warn(
							Messages.getString("Crawler.9"), //$NON-NLS-1$
							seedUrl.toString());
					// TODO: Something is fishy with the error message here,
					// check.
					continue;
				}

				try {
					host.getRobotSettings().addRules(generalUrlFilters);
					host.getRobotSettings().addRules(customUrlFilters);
				} catch (NoPageAllowedException e) {
					logger.warn(
							Messages.getString("Crawler.14"), //$NON-NLS-1$
							seedUrl.toString());
					// TODO: Something is fishy with the error message here,
					// check.
					continue;
				}

				if (host != null) {
					hosts.add(host);
				}
			}

			// Find crawl database

			Host[] hostArray = new Host[hosts.size()];
			hosts.toArray(hostArray);
			CrawlerRunnable[] crawlerRunnables = new CrawlerRunnable[hostArray.length];
			for (int i = 0; i < crawlerRunnables.length; i++) {
				Host[] threadHosts = new Host[1];
				threadHosts[0] = hostArray[i]; // temporary hax

				CrawlerRunnable crawlerRunnable = new CrawlerRunnable(i,
						crawlerConfiguration, threadHosts, fetchScheduler,
						recrawlScheduler,
						new JaccardSimilarityChecker((byte) 3));

				crawlerRunnables[i] = crawlerRunnable;
				Thread thread = new Thread(crawlerRunnable);
				crawlerRunnable.setAssociatedThread(thread);
				thread.start();
			}

			long displayStatisticsInterval = (long) crawlerConfiguration
					.getFloat("timeSettings.displayStatisticsInterval") * 1000L; //$NON-NLS-1$
			long crawlStateWriteInterval = (long) crawlerConfiguration
					.getFloat("timeSettings.crawlStateWriteInterval") * 1000L; //$NON-NLS-1$
			new Thread(new StatisticsRunnable(crawlerRunnables,
					displayStatisticsInterval), "StatisticsThread").start(); //$NON-NLS-1$
			new Thread(new CrawlStateWriterRunnable(crawlerRunnables,
					crawlStateWriteInterval), "CrawlStateWriterThread").start(); //$NON-NLS-1$

		} else {
			File crawlDatabasePath = new File(
					crawlerConfiguration.getString("paths.crawlDatabase")); //$NON-NLS-1$
			File crawlerStates[] = crawlDatabasePath
					.listFiles(new FilenameFilter() {
						@Override
						public boolean accept(File dir, String name) {
							if (name.matches("crawler[0-9]+.ser")) //$NON-NLS-1$
								return true;
							return false;
						}
					});

			CrawlerRunnable[] crawlerRunnables = new CrawlerRunnable[crawlerStates.length];

			for (File state : crawlerStates) {
				int crawlerNum = Integer.parseInt(state.getName().replaceAll(
						"crawler([0-9]+).ser", "$1")); //$NON-NLS-1$ //$NON-NLS-2$
				try {
					crawlerRunnables[crawlerNum] = (CrawlerRunnable) FileUtils
							.readObjectFromFile(state);
				} catch (ClassNotFoundException e) {
					logger.error(
							Messages.getString("Crawler.23"), //$NON-NLS-1$
							state.toString());
					logger.debug(ExceptionUtils.getStackTrace(e));
				} catch (IOException e) {
					logger.error(
							Messages.getString("Crawler.24"), //$NON-NLS-1$
							state.toString());
					logger.debug(ExceptionUtils.getStackTrace(e));
				}

				Thread thread = new Thread(crawlerRunnables[crawlerNum]);
				crawlerRunnables[crawlerNum].setAssociatedThread(thread);
				crawlerRunnables[crawlerNum].reinitializeTransients();
				thread.start();
			}
			long displayStatisticsInterval = (long) crawlerConfiguration
					.getFloat("timeSettings.displayStatisticsInterval") * 1000L; //$NON-NLS-1$
			long crawlStateWriteInterval = (long) crawlerConfiguration
					.getFloat("timeSettings.crawlStateWriteInterval") * 1000L; //$NON-NLS-1$
			new Thread(new StatisticsRunnable(crawlerRunnables,
					displayStatisticsInterval), "StatisticsThread").start(); //$NON-NLS-1$
			new Thread(new CrawlStateWriterRunnable(crawlerRunnables,
					crawlStateWriteInterval), "CrawlStateWriterThread").start(); //$NON-NLS-1$
		}

	}

	static ArrayList<RobotRule> readUrlFilters(SubnodeConfiguration filterConfiguration) {
		ArrayList<RobotRule> urlFilters = new ArrayList<RobotRule>();
		List<HierarchicalConfiguration> disallows = filterConfiguration
				.configurationsAt("disallow"); //$NON-NLS-1$

		for (HierarchicalConfiguration disallow : disallows) {
			String ruleString = disallow.getString(""); //$NON-NLS-1$
			Rule ruleType = Rule.DISALLOW;
			Pattern expression = Pattern.compile(ruleString);
			RobotRule robotRule = new RobotRule(ruleType, expression);
			urlFilters.add(robotRule);
		}

		List<HierarchicalConfiguration> allows = filterConfiguration
				.configurationsAt("allow"); //$NON-NLS-1$
		for (HierarchicalConfiguration allow : allows) {
			String ruleString = allow.getString(""); //$NON-NLS-1$
			Rule ruleType = Rule.ALLOW;
			Pattern expression = Pattern.compile(ruleString);
			RobotRule robotRule = new RobotRule(ruleType, expression);
			urlFilters.add(robotRule);
		}

		return urlFilters;
	}

}
