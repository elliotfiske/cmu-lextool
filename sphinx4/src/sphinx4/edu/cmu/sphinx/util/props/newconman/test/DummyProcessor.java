package edu.cmu.sphinx.util.props.newconman.test;

import edu.cmu.sphinx.util.props.PropSheet;
import edu.cmu.sphinx.util.props.PropertyException;

/**
 * DOCUMENT ME!
 *
 * @author Holger Brandl
 */
public class DummyProcessor implements DummyFrontEndProcessor {


    public void newProperties(PropSheet ps) throws PropertyException {
    }


    public String getName() {
        return this.getClass().getName();
    }
}
