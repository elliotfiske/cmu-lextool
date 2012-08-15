package edu.cmu.sphinx.sphingid.robots;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.util.ArrayList;
import java.util.regex.Pattern;

import org.apache.commons.lang3.exception.ExceptionUtils;
import org.junit.Test;

import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule.Rule;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotSettings.NoPageAllowedException;

/**
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class RobotSettingsTest {
	@SuppressWarnings("static-method")
	@Test
	public void canRemoveDuplicates() {
		ArrayList<RobotRule> ruleList = new ArrayList<RobotRule>();

		ruleList.add(new RobotRule(Rule.DISALLOW, "/")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.ALLOW, "/")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.ALLOW, "/url1")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.DISALLOW, "/url1")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.DISALLOW, "/url2")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.DISALLOW, "/url2")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.ALLOW, "/url3")); //$NON-NLS-1$
		ruleList.add(new RobotRule(Rule.ALLOW, "/url3")); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = new RobotSettings(ruleList, null, 5);
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		ArrayList<RobotRule> noDupRuleList = robotSettings.getRuleList();
		assertThat(noDupRuleList.size(), is(4));

		assertThat(noDupRuleList.get(0).getRuleType(), is(Rule.DISALLOW));
		assertThat(noDupRuleList.get(1).getRuleType(), is(Rule.DISALLOW));
		assertThat(noDupRuleList.get(2).getRuleType(), is(Rule.ALLOW));
		assertThat(noDupRuleList.get(3).getRuleType(), is(Rule.DISALLOW));
	}

	@SuppressWarnings({ "static-method", "unused" })
	@Test(expected = NoPageAllowedException.class)
	public void canThrowNoPageAllowedException() throws NoPageAllowedException {
		ArrayList<RobotRule> ruleList = new ArrayList<RobotRule>();

		ruleList.add(new RobotRule(Rule.DISALLOW, "/")); //$NON-NLS-1$

		new RobotSettings(ruleList, null, 5);
	}

	@SuppressWarnings("static-method")
	@Test
	public void noAllows() {
		ArrayList<RobotRule> ruleList = new ArrayList<RobotRule>();

		ruleList.add(new RobotRule(Rule.DISALLOW, "/url1")); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = new RobotSettings(ruleList, null, 5);
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertFalse(robotSettings.accepts("/url1")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("/url2")); //$NON-NLS-1$
	}

	@SuppressWarnings("static-method")
	@Test
	public void noDisallows() {
		ArrayList<RobotRule> ruleList = new ArrayList<RobotRule>();

		ruleList.add(new RobotRule(Rule.ALLOW, "/url1")); //$NON-NLS-1$

		RobotSettings robotSettings = null;
		try {
			robotSettings = new RobotSettings(ruleList, null, 5);
		} catch (NoPageAllowedException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertTrue(robotSettings.accepts("/url1")); //$NON-NLS-1$
		assertTrue(robotSettings.accepts("/url2")); //$NON-NLS-1$
	}
}
