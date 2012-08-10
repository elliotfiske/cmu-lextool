/**
 * 
 */
package edu.cmu.sphinx.sphingid.crawler;

import static org.hamcrest.Matchers.is;
import static org.junit.Assert.assertThat;
import static org.junit.Assert.fail;

import java.net.MalformedURLException;
import java.net.URL;
import java.util.ArrayList;

import org.apache.commons.lang.exception.ExceptionUtils;
import org.junit.Test;

/**
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 * 
 */
public class ParserTest {
	@SuppressWarnings("static-method")
	@Test
	public void canGenerateUrlsCorrectly() {
		String url = null;

		try {
			url = Parser.generateUrl(new URL("http://www.test.com"), //$NON-NLS-1$
					"http://www.test.com/url1"); //$NON-NLS-1$
		} catch (MalformedURLException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(url, is("http://www.test.com/url1")); //$NON-NLS-1$

		try {
			url = Parser.generateUrl(new URL("http://www.test.com"), //$NON-NLS-1$
					"?samplequery"); //$NON-NLS-1$
		} catch (MalformedURLException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(url, is("http://www.test.com/?samplequery")); //$NON-NLS-1$

		try {
			url = Parser.generateUrl(new URL("http://www.test.com"), //$NON-NLS-1$
					"ftp://www.test.com/url2"); //$NON-NLS-1$
		} catch (MalformedURLException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(url, is((String) null));

		try {
			url = Parser.generateUrl(new URL("http://www.test.com"), //$NON-NLS-1$
					"http://www.anothertest.com"); //$NON-NLS-1$
		} catch (MalformedURLException e) {
			fail(ExceptionUtils.getStackTrace(e));
		}

		assertThat(url, is((String) null));
	}

	@SuppressWarnings("static-method")
	@Test
	public void canRemoveDuplicates() {
		ArrayList<String> list = new ArrayList<String>();

		list.add("a"); //$NON-NLS-1$
		list.add("c"); //$NON-NLS-1$
		list.add("b"); //$NON-NLS-1$
		list.add("b"); //$NON-NLS-1$
		list.add("a"); //$NON-NLS-1$
		list.add("c"); //$NON-NLS-1$

		Parser.removeDuplicates(list);

		assertThat(list.size(), is(3));
		assertThat(list.get(0), is("a")); //$NON-NLS-1$
		assertThat(list.get(1), is("b")); //$NON-NLS-1$
		assertThat(list.get(2), is("c")); //$NON-NLS-1$
	}
}
