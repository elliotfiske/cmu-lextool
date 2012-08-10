/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;
import static org.mockito.Mockito.mock;
import static org.mockito.Mockito.when;

import java.util.ArrayList;
import java.util.NoSuchElementException;

import org.junit.Before;
import org.junit.Test;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class LinkQueueTest {
	private Host testHost;
	private long testTime;

	@Before
	public void initializeHost() {
		this.testHost = mock(Host.class);
		when(this.testHost.getCrawlDelay()).thenReturn(5000L);
	}

	@Before
	public void initializeTestTime() {
		this.testTime = System.currentTimeMillis();
	}

	@Test(expected = NoSuchElementException.class)
	public void canThrowNoSuchElementException() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.peek();
	}

	@Test(expected = NoSuchElementException.class)
	public void canThrowExceptionWhenPolledWhileEmpty() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.poll();
	}

	@Test
	public void canPollCorrectly() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.add(1, 40);
		queue.add(2, 50);
		assertThat(queue.poll(), is(1));
		assertThat(queue.peek(), is(2));
	}

	@Test
	public void canPeekCorrectly() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.add(1, 40);
		queue.add(2, 50);
		assertThat(queue.peek(), is(1));
	}

	@Test
	public void returnsSizeCorrectly() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.add(1, 40);
		queue.add(2, 50);
		assertThat(queue.size(), is(2));
	}

	@Test
	public void returnsIsEmptyCorrectly() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		assertThat(queue.isEmpty(), is(true));
		queue.add(1, 40);
		queue.add(2, 50);
		assertThat(queue.isEmpty(), is(false));
	}

	@Test
	public void isSortedAfterAddWithTime() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.add(1, 40);
		queue.add(2, 50);

		assertThat(queue.poll(), is(1));
		assertThat(queue.poll(), is(2));

		queue = new LinkQueue(this.testHost, this.testTime);

		queue.add(1, 50);
		queue.add(2, 40);

		assertThat(queue.poll(), is(2));
		assertThat(queue.poll(), is(1));

		queue = new LinkQueue(this.testHost, this.testTime);

		queue.add(1, 40);
		queue.add(2, 40);
		queue.add(3, 50);
		queue.add(4, 10);

		assertThat(queue.poll(), is(4));
		assertThat(queue.poll(), is(1));
		assertThat(queue.poll(), is(2));
		assertThat(queue.poll(), is(3));
	}

	@Test
	public void canScheduleNewEntriesCorrectly() {

		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.add(3,
				this.testTime + (long) (2.5F * this.testHost.getCrawlDelay()));
		queue.add(1);
		queue.add(2);
		queue.add(4);

		assertThat(queue.peekFetchTime(),
				is(this.testTime + this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(1));
		assertThat(queue.peekFetchTime(),
				is(this.testTime + 2 * this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(2));
		assertThat(queue.peekFetchTime(), is(this.testTime
				+ (long) (2.5F * this.testHost.getCrawlDelay())));
		assertThat(queue.poll(), is(3));
		assertThat(queue.peekFetchTime(),
				is(this.testTime + 3 * this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(4));
	}

	@Test
	public void canAddAllCorrectly() {
		LinkQueue queue = new LinkQueue(this.testHost, this.testTime);
		queue.add(3,
				this.testTime + (long) (2.5F * this.testHost.getCrawlDelay()));
		ArrayList<Integer> list = new ArrayList<Integer>();
		list.add(1);
		list.add(2);
		list.add(4);
		queue.addAll(list);

		assertThat(queue.peekFetchTime(),
				is(this.testTime + this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(1));
		assertThat(queue.peekFetchTime(),
				is(this.testTime + 2 * this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(2));
		assertThat(queue.peekFetchTime(), is(this.testTime
				+ (long) (2.5F * this.testHost.getCrawlDelay())));
		assertThat(queue.poll(), is(3));
		assertThat(queue.peekFetchTime(),
				is(this.testTime + 3 * this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(4));

		queue = new LinkQueue(this.testHost, this.testTime);
		queue.addAll(list);
		assertThat(queue.peekFetchTime(),
				is(this.testTime + this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(1));
		assertThat(queue.peekFetchTime(),
				is(this.testTime + 2 * this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(2));
		assertThat(queue.peekFetchTime(),
				is(this.testTime + 3 * this.testHost.getCrawlDelay()));
		assertThat(queue.poll(), is(4));
	}
}
