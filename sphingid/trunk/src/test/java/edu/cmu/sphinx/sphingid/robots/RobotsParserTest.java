package edu.cmu.sphinx.sphingid.robots;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.io.File;
import java.io.IOException;
import java.net.URL;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.junit.Test;

import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings.NoPageAllowedException;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotsParser.CrawlDelayException;

/**
 * 
 */

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class RobotsParserTest {
	@Test
	public void canAcceptGeneralRules() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsGeneralRulesTest/robots.txt"); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/url1")); //$NON-NLS-1$
		assertThat(robotSettings.getCrawlDelay(), is(12000L));
	}

	@Test
	public void canCorrectlyParseRegularExpressions() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsRegexTest/robots.txt"); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/url1/image.gif")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url1/text.txt")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/image.gif")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/image.jpg")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url2/image.jpg")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url1/image.jpg")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/forbidden.jpg")); //$NON-NLS-1$
		assertFalse(robotSettings
				.accepts("http://www.test.com/url1/forbidden.jpg")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/str")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/astr")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/x.str")); //$NON-NLS-1$
	}

	@Test
	public void canGetInverseSpecializedRules() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsInverseSpecializedRuleTest/robots.txt"); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/url4")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url5")); //$NON-NLS-1$

		/*
		 * Accept an allowed rule too
		 */
		assertTrue(robotSettings.accepts("http://www.test.com/url6")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/")); //$NON-NLS-1$

		assertThat(robotSettings.getCrawlDelay(), is(20000L));

	}

	@Test
	public void canGetMalformedInput() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsInputTest" + File.separator + "robots.txt"); //$NON-NLS-1$ //$NON-NLS-2$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/url1")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url10")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url2")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url3")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url4")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url4/url5")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url4/url10")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url4/url7")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url4/url5/url6")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url4/url5/url8")); //$NON-NLS-1$
		assertTrue(robotSettings
				.accepts("http://www.test.com/url4/url5/url8/url9")); //$NON-NLS-1$
		assertFalse(robotSettings
				.accepts("http://www.test.com/url4/url5/url6/url10")); //$NON-NLS-1$
		assertTrue(robotSettings.getRuleList().size() == 6);
		assertThat(robotSettings.getSitemapList().get(0),
				is("http://www.test.com/map1")); //$NON-NLS-1$
		assertThat(robotSettings.getSitemapList().get(1),
				is("http://www.test.com/map2")); //$NON-NLS-1$

	}

	@Test
	public void canGetMultipleEntries() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsMultipleEntriesTest/robots.txt"); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url4/url5")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url4/url5/url6")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/url7")); //$NON-NLS-1$
		/*
		 * Accept an allowed rule too
		 */
		assertTrue(robotSettings.accepts("http://www.test.com/url4/url5/url8")); //$NON-NLS-1$

		assertThat(robotSettings.getCrawlDelay(), is(15000L));
	}

	@Test
	public void canGetSpecializedRules() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsSpecializedRuleTest/robots.txt"); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/url4")); //$NON-NLS-1$
		assertFalse(robotSettings.accepts("http://www.test.com/url5")); //$NON-NLS-1$

		/*
		 * Accept an allowed rule too
		 */
		assertTrue(robotSettings.accepts("http://www.test.com/url6")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("http://www.test.com/")); //$NON-NLS-1$

		assertThat(robotSettings.getCrawlDelay(), is(20000L));
	}

	@Test(expected = CrawlDelayException.class)
	public void canRejectUnacceptableCrawlDelay() throws CrawlDelayException {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsCrawlDelayTest/robots.txt"); //$NON-NLS-1$

		try {
			RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

	}

	@Test
	public void canRemoveDuplicates() {
		URL testFileUrl = getClass().getClassLoader().getResource(
				"robotsDuplicateEntriesTest/robots.txt"); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = RobotsParser.getInstance().parse(testFileUrl);
		} catch (IOException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		} catch (CrawlDelayException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("http://www.test.com/url4")); //$NON-NLS-1$
	}

}
