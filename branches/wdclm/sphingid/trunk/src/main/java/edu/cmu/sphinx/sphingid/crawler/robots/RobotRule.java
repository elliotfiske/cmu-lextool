/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler.robots;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

import com.esotericsoftware.kryo.Kryo;
import com.esotericsoftware.kryo.KryoSerializable;
import com.esotericsoftware.kryo.io.Input;
import com.esotericsoftware.kryo.io.Output;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class RobotRule implements Comparable<RobotRule>, KryoSerializable {
	public static enum Rule {
		ALLOW, DISALLOW
	}

	private Rule rule;
	private Pattern expression;

	public RobotRule(Rule rule, String expressionString) {
		this.rule = rule;
		this.expression = Pattern.compile(expressionString);
	}
	
	private RobotRule() {
		super();
	}

	/**
	 * Checks if this RobotRule instance accepts a given URL.
	 * <p>
	 * If rule matches the URL and if the rule is an Allow rule, the method will
	 * return true. If the rule is a disallow rule, it must prevent the URL from
	 * being fetched, so it will return false. Similarly, if rule path does not
	 * match the URL and if the rule is an Allow rule, the method will return
	 * false. If the rule is a Disallow rule, this rule does not prevent the URL
	 * from being accepted, so it will return true.
	 * 
	 * @param url
	 *            URL to be tested
	 * @return true if the rule is an Allow rule and rule matches the URL and if
	 *         the rule is a Disallow rule and rule path does not match the URL
	 */
	public boolean accepts(String url) {
		Matcher urlMatcher = this.expression.matcher(url);

		if (urlMatcher.find()) {
			if (this.rule == RobotRule.Rule.ALLOW)
				return true;

			return false;
		}

		if (this.rule == RobotRule.Rule.ALLOW)
			return false;

		return true;

	}

	/*
	 * (non-Javadoc)
	 * 
	 * @see java.lang.Comparable#compareTo(java.lang.Object)
	 */
	/**
	 * Does comparison based solely on pattern length.
	 * 
	 * @return Returns -1 if this object has a shorter pattern length than the
	 *         target, 0 if equal, 1 if longer
	 */
	@Override
	public int compareTo(RobotRule o) {
		if (this.getExpression().pattern().length() < o.getExpression()
				.pattern().length()) {
			return -1;
		} else if (this.getExpression().pattern().length() == o.getExpression()
				.pattern().length()) {
			return 0;
		} else {
			return 1;
		}
	}

	public Pattern getExpression() {
		return this.expression;
	}

	public Rule getRuleType() {
		return this.rule;
	}

	@Override
	public void read(Kryo arg0, Input arg1) {
		this.expression = Pattern.compile(arg1.readString());
		this.rule = Rule.valueOf(arg1.readString());
		// TODO Auto-generated method stub
		
	}

	@Override
	public void write(Kryo arg0, Output arg1) {
		arg1.writeString(expression.pattern());
		arg1.writeString(rule.name());
		// TODO Auto-generated method stub
		
	}

}
