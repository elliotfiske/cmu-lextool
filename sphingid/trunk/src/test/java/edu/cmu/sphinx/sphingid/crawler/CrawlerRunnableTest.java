package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.fail;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

import org.apache.commons.configuration.XMLConfiguration;
import org.apache.commons.lang.exception.ExceptionUtils;
import org.junit.Before;
import org.junit.Test;
import org.junit.rules.TemporaryFolder;

import de.l3s.boilerpipe.BoilerpipeProcessingException;
import edu.cmu.sphinx.sphingid.commons.FileUtils;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings.NoPageAllowedException;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser.CrawlDelayException;

public class CrawlerRunnableTest {
	private CrawlerRunnable crawler;
	private TemporaryFolder tmpFolder = new TemporaryFolder();
	private File crawlDatabase;
	private RobotSettings robotSettings;

	@Before
	public void initTest() {
		File folder = null;
		try {
			folder = this.tmpFolder.newFolder();
			this.crawlDatabase = this.tmpFolder.newFolder();
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}
		XMLConfiguration configuration = new XMLConfiguration();
		configuration.setProperty("similarityThreshold", 0.5); //$NON-NLS-1$
		configuration.setProperty("crawlerName", "Sphingid-Test"); //$NON-NLS-1$ //$NON-NLS-2$
		configuration.setProperty("timeSettings.connectionTimeout", 2.0F); //$NON-NLS-1$
		configuration.setProperty("timeSettings.readTimeout", 2.0F); //$NON-NLS-1$
		configuration
				.setProperty("paths.extractedDocuments", folder.toString()); //$NON-NLS-1$
		configuration.setProperty("paths.crawlDatabase", //$NON-NLS-1$
				this.crawlDatabase.toString());

		String url = "http://sourceforge.net";

		Host host = null;
		try {
			host = new Host(url);
		} catch (MalformedURLException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			// fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		URL robotsUrl = getClass().getClassLoader().getResource(
				"robotsInputTest/robots.txt"); //$NON-NLS-1$
		this.robotSettings = null;
		try {
			this.robotSettings = RobotsParser.getInstance().parse(robotsUrl);
		} catch (NumberFormatException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (MalformedURLException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		Host hosts[] = new Host[1];
		hosts[0] = host;

		FetchScheduler fetchScheduler = new EarliestDeadlineFirstScheduler();
		RecrawlScheduler recrawlScheduler = new SimpleRecrawlScheduler(5000,
				10000, 2);
		DocumentSimilarityChecker checker = new JaccardSimilarityChecker(
				(byte) 3);
		this.crawler = new CrawlerRunnable(0, configuration, hosts,
				fetchScheduler, recrawlScheduler, checker);
	}

	@Test
	public void canCalculateMovingAverageCorrectly() {
		for (int i = 0; i < 50; i++) {
			this.crawler.calculateMovingAverage(50, 100L);
		}

		this.crawler.calculateMovingAverage(100, 200L);

		assertThat(this.crawler.getMovingWordAverage(),
				is((50 * 49 + 1 * 100) / 50.0F));
		assertThat(this.crawler.getFetchDurationAverage(),
				is((100 * 49 + 1 * 200) / 50.0F));
	}

	@Test
	public void canGetAndUpdateWebpage() {
		/*
		 * Testing all branches
		 */
		Page page = new Page(17, "checksum", "lorem ipsum", 2000L); //$NON-NLS-1$ //$NON-NLS-2$
		File pagesFolder = new File(this.crawlDatabase, "pages"); //$NON-NLS-1$
		File pageFile = new File(pagesFolder, "0pf17"); //$NON-NLS-1$

		try {
			FileUtils.serializeObject(pageFile, page);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		page = null;

		try {
			page = this.crawler.getAndUpdatePage(4000L, 17, "checksum2", //$NON-NLS-1$
					"dolor sit amet"); //$NON-NLS-1$
		} catch (BoilerpipeProcessingException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(page.getLastFetchTime(), is(4000L));
		assertThat(page.getChecksum(), is("checksum2")); //$NON-NLS-1$
		assertThat(page.getAllTextContent(), is("dolor sit amet\n")); //$NON-NLS-1$

		page = new Page(17, "checksum", "lorem ipsum", 2000L); //$NON-NLS-1$ //$NON-NLS-2$
		pageFile = new File(pagesFolder, "0pf17"); //$NON-NLS-1$

		try {
			FileUtils.serializeObject(pageFile, page);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			page = this.crawler.getAndUpdatePage(4000L, 17, "checksum", //$NON-NLS-1$
					"lorem ipsum"); //$NON-NLS-1$
		} catch (BoilerpipeProcessingException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(page.getLastFetchTime(), is(4000L));
		assertThat(page.getChecksum(), is("checksum")); //$NON-NLS-1$
		assertThat(page.getAllTextContent(), is("lorem ipsum")); //$NON-NLS-1$

		page = new Page(17, "checksum", "lorem ipsum", 2000L); //$NON-NLS-1$ //$NON-NLS-2$
		pageFile = new File(this.crawlDatabase, "0pf17"); //$NON-NLS-1$

		try {
			FileUtils.serializeObject(pageFile, page);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		try {
			page = this.crawler.getAndUpdatePage(4000L, 17, "checksum2", //$NON-NLS-1$
					"lorem ipsum"); //$NON-NLS-1$
		} catch (BoilerpipeProcessingException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(page.getLastFetchTime(), is(4000L));
		assertThat(page.getChecksum(), is("checksum2")); //$NON-NLS-1$
		assertThat(page.getAllTextContent(), is("lorem ipsum")); //$NON-NLS-1$
	}

	@Test
	public void canReadContent() {
		try {
			this.crawler.readContent("http://sourceforge.net"); //$NON-NLS-1$
		} catch (Exception e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

	}

	@Test
	public void canRemoveDisallowedAndOutgoing() {
		ArrayList<String> urls = new ArrayList<String>();
		urls.add("http://www.test.com/url1"); //$NON-NLS-1$
		urls.add("http://www.test.com/url6"); //$NON-NLS-1$
		urls.add("http://www.test.com/url2"); //$NON-NLS-1$
		urls.add("http://www.test.com/url4/url5"); //$NON-NLS-1$

		Host host = mock(Host.class);
		when(host.getRobotSettings()).thenReturn(this.robotSettings);
		when(host.getUrl()).thenReturn("http://www.test.com"); //$NON-NLS-1$

		CrawlerRunnable.removeDisallowedAndOutgoing(urls, host);

		assertThat(urls.contains("http://www.test.com/url6"), is(true)); //$NON-NLS-1$
		assertThat(urls.contains("http://www.test.com/url4/url5"), is(true)); //$NON-NLS-1$
		assertThat(urls.size(), is(2));

	}

	@Test
	public void canSerialize() {

		File crawlerStateFile = new File(this.crawlDatabase, "crawler0.ser"); //$NON-NLS-1$

		try {
			FileUtils.serializeObject(crawlerStateFile, this.crawler);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		CrawlerRunnable newCrawler = null;

		try {
			newCrawler = FileUtils.deserializeObject(crawlerStateFile,
					CrawlerRunnable.class);
		} catch (FileNotFoundException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(newCrawler.getCrawlerNum(), is(0));
		assertThat(newCrawler.getUrlTable().getUrl(0),
				is("http://sourceforge.net")); //$NON-NLS-1$
	}

	
}
