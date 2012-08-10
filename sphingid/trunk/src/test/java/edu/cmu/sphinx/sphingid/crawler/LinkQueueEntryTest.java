/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;

import org.junit.Test;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class LinkQueueEntryTest {
	@SuppressWarnings("static-method")
	@Test
	public void canCompareCorrectly() {
		LinkQueueEntry linkQueueEntry1 = new LinkQueueEntry(1, 20L);
		LinkQueueEntry linkQueueEntry2 = new LinkQueueEntry(2, 20L);
		LinkQueueEntry linkQueueEntry3 = new LinkQueueEntry(3, 40L);

		assertThat(linkQueueEntry1.compareTo(linkQueueEntry3), is(-1));
		assertThat(linkQueueEntry1.compareTo(linkQueueEntry2), is(0));
		assertThat(linkQueueEntry3.compareTo(linkQueueEntry1), is(1));
	}
}
