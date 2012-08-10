/**
 * 
 */
package edu.cmu.sphinx.sphingid.robots;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;

import java.util.regex.Pattern;

import org.junit.Test;

import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule;
import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule.Rule;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class RobotRuleTest {

	@SuppressWarnings("static-method")
	@Test
	public void canAcceptCorrectly() {
		RobotRule rule = new RobotRule(Rule.DISALLOW, Pattern.compile("/url1")); //$NON-NLS-1$

		assertThat(rule.accepts("http://www.test.com/"), is(true)); //$NON-NLS-1$
		assertThat(rule.accepts("http://www.test.com/url1"), is(false)); //$NON-NLS-1$
		assertThat(rule.accepts("http://www.test.com/url1/url2"), is(false)); //$NON-NLS-1$

		rule = new RobotRule(Rule.ALLOW, Pattern.compile("/url1")); //$NON-NLS-1$

		assertThat(rule.accepts("http://www.test.com/"), is(false)); //$NON-NLS-1$
		assertThat(rule.accepts("http://www.test.com/url1"), is(true)); //$NON-NLS-1$
		assertThat(rule.accepts("http://www.test.com/url1/url2"), is(true)); //$NON-NLS-1$

	}

	@SuppressWarnings("static-method")
	@Test
	public void canCompare() {
		RobotRule rule1 = new RobotRule(Rule.DISALLOW, Pattern.compile("/url1")); //$NON-NLS-1$
		RobotRule rule2 = new RobotRule(Rule.DISALLOW,
				Pattern.compile("/url1/url2")); //$NON-NLS-1$
		RobotRule rule3 = new RobotRule(Rule.DISALLOW, Pattern.compile("/url3")); //$NON-NLS-1$

		assertThat(rule1.compareTo(rule2), is(-1));
		assertThat(rule2.compareTo(rule1), is(1));
		assertThat(rule1.compareTo(rule3), is(0));
	}
}
