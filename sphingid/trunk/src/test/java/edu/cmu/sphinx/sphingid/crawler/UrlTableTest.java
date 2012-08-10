/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;

import org.junit.Test;

/**
 * @author Anubis
 * 
 */
public class UrlTableTest {
	@SuppressWarnings("static-method")
	@Test
	public void canAdd() {
		UrlTable urlTable = new UrlTable(100);

		int pos = urlTable.add("a"); //$NON-NLS-1$
		assertThat(pos, is(0));
		assertThat(urlTable.containsUrl("a"), is(true)); //$NON-NLS-1$
		assertThat(urlTable.getUrl(0), is("a")); //$NON-NLS-1$

		pos = urlTable.add("b"); //$NON-NLS-1$
		assertThat(pos, is(1));
		assertThat(urlTable.containsUrl("b"), is(true)); //$NON-NLS-1$
		assertThat(urlTable.getUrl(0), is("a")); //$NON-NLS-1$
		assertThat(urlTable.getUrl(1), is("b")); //$NON-NLS-1$

		pos = urlTable.add("a"); //$NON-NLS-1$
		assertThat(pos, is(-1));
		assertThat(urlTable.containsUrl("a"), is(true)); //$NON-NLS-1$
		assertThat(urlTable.getUrl(0), is("a")); //$NON-NLS-1$
		assertThat(urlTable.getUrl(1), is("b")); //$NON-NLS-1$
	}

}
