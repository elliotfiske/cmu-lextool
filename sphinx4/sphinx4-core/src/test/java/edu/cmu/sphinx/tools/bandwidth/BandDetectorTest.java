/*
 * Copyright 1999-2013 Carnegie Mellon University. All Rights Reserved. Use is
 * subject to license terms. See the file "license.terms" for information on
 * usage and redistribution of this file, and for a DISCLAIMER OF ALL
 * WARRANTIES.
 */

package edu.cmu.sphinx.tools.bandwidth;

import static com.google.common.io.Resources.getResource;
import static org.testng.Assert.assertFalse;
import static org.testng.Assert.assertTrue;

import org.testng.annotations.Test;


public class BandDetectorTest {

    @Test
    public void test() {
        BandDetector detector = new BandDetector();
        assertTrue(detector
                .bandwidth(getResource(getClass(),
                                       "10001-90210-01803-8khz.wav")
                        .getPath()));
        assertFalse(detector
                .bandwidth(getResource(getClass(), "10001-90210-01803.wav")
                        .getPath()));
    }
}
