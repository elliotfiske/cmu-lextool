package edu.cmu.sphinx.util.props.newconman.test;

import edu.cmu.sphinx.util.props.*;
import edu.cmu.sphinx.util.props.newconman.test.DummyProcessor;
import edu.cmu.sphinx.util.props.newconman.test.AnotherDummyProcessor;

import java.util.List;

/**
 * DOCUMENT ME!
 *
 * @author Holger Brandl
 */
public class DummyFrontEnd implements Configurable {

    @S4Boolean(defaultValue = true)
    public static final String PROP_USE_MFFCS = "useMfccs";

    @S4ComponentList(
            type = Configurable.class,
            defaultList = {DummyProcessor.class, AnotherDummyProcessor.class, DummyProcessor.class}
    )
    public static final String DATA_PROCS = "dataProcs";

    List<Configurable> dataProcs;

    boolean useMfccs;


    public void register(String name, Registry registry) throws PropertyException {
    }


    public void newProperties(PropertySheet ps) throws PropertyException {
        useMfccs = ps.getBoolean(PROP_USE_MFFCS);
        dataProcs = ps.getComponentList(DATA_PROCS);
    }


    public boolean isUseMfccs() {
        return useMfccs;
    }


    public List<Configurable> getDataProcs() {
        return dataProcs;
    }


    public String getName() {
        return this.getClass().getName();
    }
}
