package edu.cmu.sphinx.sphingid.crawler.robots;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collections;

import edu.cmu.sphinx.sphingid.crawler.robots.RobotRule.Rule;

/**
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 *
 */
public class RobotSettings implements Serializable {
	public class NoPageAllowedException extends Exception {

		/**
		 * 
		 */
		private static final long serialVersionUID = 8376081880267522399L;

	}

	private static final long serialVersionUID = -8901367150589299758L;
	private ArrayList<RobotRule> ruleList;
	private ArrayList<String> sitemapList;

	private long crawlDelay;

	public RobotSettings(ArrayList<RobotRule> ruleList,
			ArrayList<String> sitemapList, long crawlDelay)
			throws NoPageAllowedException {
		this.ruleList = new ArrayList<RobotRule>();
		addRules(ruleList);

		this.sitemapList = sitemapList;
		this.crawlDelay = crawlDelay;
	}

	public boolean accepts(String url) {
		int mostRestrictedAllow = -1, mostRestrictedDisallow = -1;
		for (int i = 0; i < this.ruleList.size(); i++) {
			if (this.ruleList.get(i).getRuleType() == Rule.DISALLOW) {
				if (!this.ruleList.get(i).accepts(url)) {
					mostRestrictedDisallow = i;
					break;
				}
			}
		}

		if (mostRestrictedDisallow == -1)
			return true;

		for (int i = 0; i < this.ruleList.size(); i++) {
			if (this.ruleList.get(i).getRuleType() == Rule.ALLOW) {
				if (this.ruleList.get(i).accepts(url)) {
					mostRestrictedAllow = i;
					break;
				}
			}
		}

		if (mostRestrictedAllow == -1)
			return false;

		if (this.ruleList.get(mostRestrictedAllow).compareTo(
				this.ruleList.get(mostRestrictedDisallow)) == 1) {
			return true;
		}

		return false;
	}

	public void addRules(ArrayList<RobotRule> ruleList)
			throws NoPageAllowedException {
		@SuppressWarnings("unchecked")
		ArrayList<RobotRule> result = (ArrayList<RobotRule>) ruleList.clone();

		/*
		 * Remove duplicate paths, prefer disallow over allow
		 */
		for (int i = 0; i < result.size() - 1; ++i) {
			RobotRule rule = result.get(i);
			for (int j = i + 1; j < result.size(); ++j) {
				RobotRule anotherRule = result.get(j);
				if (rule.getExpression().pattern()
						.equals(anotherRule.getExpression().pattern())) {
					if (rule.getRuleType() == RobotRule.Rule.DISALLOW) {
						result.remove(j);
						--j;
					} else {
						result.remove(i);
						--i;
						--j;
					}

				}
			}
		}

		/*
		 * Check if everything is disallowed, i.e. Disallow: / and no allows
		 */
		boolean disallowAll = false;
		for (RobotRule rule : result) {
			if (rule.getRuleType() == RobotRule.Rule.DISALLOW
					&& rule.getExpression().pattern().equals("/")) { //$NON-NLS-1$
				disallowAll = true;
				break;
			}
		}

		if (disallowAll) {
			for (RobotRule rule : result) {
				if (rule.getRuleType() == RobotRule.Rule.ALLOW) {
					disallowAll = false;
					break;
				}
			}
		}

		if (disallowAll)
			throw new NoPageAllowedException();

		this.ruleList.addAll(result);
		Collections.sort(this.ruleList, Collections.reverseOrder());
	}

	public long getCrawlDelay() {
		return this.crawlDelay;
	}

	public ArrayList<RobotRule> getRuleList() {
		return this.ruleList;
	}

	public ArrayList<String> getSitemapList() {
		return this.sitemapList;
	}
}
