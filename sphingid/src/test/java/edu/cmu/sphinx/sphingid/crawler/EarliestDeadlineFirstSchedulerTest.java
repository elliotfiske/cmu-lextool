/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;
import static org.mockito.Mockito.when;

import org.junit.Before;
import org.junit.Test;
import org.mockito.Mock;
import org.mockito.MockitoAnnotations;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class EarliestDeadlineFirstSchedulerTest {
	private EarliestDeadlineFirstScheduler scheduler;
	private long testTime;
	@Mock
	LinkQueue queue1, queue2;

	@Before
	public void initializeScheduler() {
		this.scheduler = new EarliestDeadlineFirstScheduler();
	}

	@Before
	public void initializeMocks() {
		MockitoAnnotations.initMocks(this);
	}

	@Before
	public void initializeTime() {
		this.testTime = System.currentTimeMillis();
	}

	@Test
	public void canGetNextFetchTimeCorrectly() {
		when(this.queue1.getCrawlDelay()).thenReturn(5000L);
		when(this.queue1.getLastFetchDuration()).thenReturn(1000L);

		/*
		 * First case: Where crawl delay is already over but the page must be
		 * fetched in future
		 */

		when(this.queue1.getLastFetchTime()).thenReturn(this.testTime - 200000L);
		when(this.queue1.peekFetchTime()).thenReturn(this.testTime + 200000L);

		assertThat(this.scheduler.getSleepAmountUntilNextFetch(this.queue1, this.testTime),
				is(200000L));

		/*
		 * Second case: Where crawl delay is not over yet but the page must have
		 * been fetched long ago
		 */

		when(this.queue1.getLastFetchTime()).thenReturn(this.testTime - 500L);
		when(this.queue1.peekFetchTime()).thenReturn(this.testTime - 200000L);

		assertThat(this.scheduler.getSleepAmountUntilNextFetch(this.queue1, this.testTime),
				is(5L * 1000L - 500L));

		/*
		 * Third case: The page is ready to fetch
		 */

		when(this.queue1.getLastFetchTime()).thenReturn(this.testTime - 200000L);
		when(this.queue1.peekFetchTime()).thenReturn(this.testTime - 100000L);

		assertThat(this.scheduler.getSleepAmountUntilNextFetch(this.queue1, this.testTime),
				is(0L));
	}

	@Test
	public void canGetNextDeadlineCorrectly() {
		when(this.queue1.getCrawlDelay()).thenReturn(5000L);
		when(this.queue1.getLastFetchDuration()).thenReturn(1000L);

		/*
		 * First case: Where crawl delay is already over but the page must be
		 * fetched in future
		 */

		when(this.queue1.getLastFetchTime()).thenReturn(this.testTime - 200000L);
		when(this.queue1.peekFetchTime()).thenReturn(this.testTime + 200000L);

		assertThat(this.scheduler.getNextFetchTime(this.queue1),
				is(this.testTime + 200000L + 1000L));

		/*
		 * Second case: Where crawl delay is not over yet but the page must have
		 * been fetched long ago
		 */

		when(this.queue1.getLastFetchTime()).thenReturn(this.testTime - 500L);
		when(this.queue1.peekFetchTime()).thenReturn(this.testTime - 200000L);

		assertThat(this.scheduler.getNextFetchTime(this.queue1), is(this.testTime + 5L * 1000L
				- 500L + 1000L));
	}

	@Test
	public void canGetNextQueueCorrectly() {
		LinkQueue queues[] = { this.queue1, this.queue2 };

		queues[0] = this.queue1;
		queues[1] = this.queue2;

		when(this.queue1.getCrawlDelay()).thenReturn(5000L);
		when(this.queue2.getCrawlDelay()).thenReturn(10000L);

		when(this.queue1.getLastFetchDuration()).thenReturn(1000L);
		when(this.queue2.getLastFetchDuration()).thenReturn(5000L);
		when(this.queue1.getLastFetchTime()).thenReturn(this.testTime - 200000L);
		when(this.queue2.getLastFetchTime()).thenReturn(this.testTime - 200000L);

		/*
		 * First case: One of the tasks has an earlier deadline but starts in
		 * the future. We choose the one that is ready to fetch.
		 */

		when(this.queue1.peekFetchTime()).thenReturn(this.testTime + 2000L);
		when(this.queue2.peekFetchTime()).thenReturn(this.testTime);

		assertThat(this.scheduler.getNextQueue(queues), is(0));

		/*
		 * Second case: Both tasks are ready to fetch, one has an earlier
		 * deadline.
		 */

		when(this.queue1.peekFetchTime()).thenReturn(this.testTime - 10000L);
		when(this.queue2.peekFetchTime()).thenReturn(this.testTime - 20000L);

		assertThat(this.scheduler.getNextQueue(queues), is(1));

		/*
		 * None of the tasks are ready to fetch, still we choose the one that
		 * has the earliest deadline
		 */

		when(this.queue1.peekFetchTime()).thenReturn(this.testTime + 10000L);
		when(this.queue2.peekFetchTime()).thenReturn(this.testTime + 20000L);

		assertThat(this.scheduler.getNextQueue(queues), is(0));
	}

}
