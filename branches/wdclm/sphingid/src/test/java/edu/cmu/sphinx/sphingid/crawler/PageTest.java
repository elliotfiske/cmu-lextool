/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class PageTest {
	@SuppressWarnings("static-method")
	@Test
	public void canReturnIsFetchedOnlyOnceCorrectly() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		assertTrue(page.isFetchedOnlyOnce());

		page.updateFetchTimes(3000L, true);

		assertFalse(page.isFetchedOnlyOnce());
	}

	@SuppressWarnings("static-method")
	@Test
	public void canGetIsUpdatedLastFetchCorrectly() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		assertTrue(page.isUpdatedLastFetch());

		page.updateFetchTimes(3000L, true);

		assertTrue(page.isUpdatedLastFetch());

		page.updateFetchTimes(4000L, false);

		assertFalse(page.isUpdatedLastFetch());
	}

	@SuppressWarnings("static-method")
	@Test
	public void getterSetterTest() {
		Page page = new Page(0, "checksum", "lorem ipsum dolor sit amet", 2000L); //$NON-NLS-1$ //$NON-NLS-2$

		assertThat(page.getUrl(), is(0));
		assertThat(page.getChecksum(), is("checksum")); //$NON-NLS-1$
		assertThat(page.getLastFetchTime(), is(2000L));
		assertThat(page.getAllTextContent(), is("lorem ipsum dolor sit amet")); //$NON-NLS-1$
	}

}
