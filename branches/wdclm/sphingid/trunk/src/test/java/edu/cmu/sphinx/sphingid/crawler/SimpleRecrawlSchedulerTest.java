/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;

import org.junit.Before;
import org.junit.Test;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class SimpleRecrawlSchedulerTest {
	SimpleRecrawlScheduler scheduler;

	@Test
	public void canDecreaseRecrawlInterval() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		page.updateFetchTimes(2200L, true);

		assertThat(this.scheduler.getNextRecrawlTime(page),
				is(2200L + 200L / 3L));
	}

	@Test
	public void canIncreaseRecrawlInterval() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		page.updateFetchTimes(2200L, false);

		assertThat(this.scheduler.getNextRecrawlTime(page), is(2800L));
	}

	@Test
	public void cannotDecreaseLowerThanMinimum() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		page.updateFetchTimes(2100L, true);

		assertThat(this.scheduler.getNextRecrawlTime(page), is(2100L + 50L));
	}

	@Test
	public void canReturnDefaultIntervalForPagesThatWereFetchedOnlyOnce() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		assertThat(this.scheduler.getNextRecrawlTime(page), is(2200L));
	}

	@Before
	public void initializeScheduler() {
		this.scheduler = new SimpleRecrawlScheduler(200L, 50L, 3);
	}

}
