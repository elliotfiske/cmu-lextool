package edu.cmu.sphinx.alignment;

import static com.google.common.collect.Lists.newArrayList;
import static org.hamcrest.MatcherAssert.assertThat;
import static org.hamcrest.Matchers.contains;

import java.util.List;
import java.util.Random;

import org.testng.annotations.BeforeClass;
import org.testng.annotations.Test;

import com.google.common.primitives.Ints;


public class TextAlignerLargeTest {

    private List<String> database;
    private LongTextAligner aligner;

    @BeforeClass
    public void setUp() {
        Random rng = new Random(42);
        database = newArrayList();
        String[] dictionary = new String[] {"foo", "bar", "baz", "quz"};
        for (int i = 0; i < 100000; ++i)
            database.add(dictionary[rng.nextInt(dictionary.length)]);
        aligner = new LongTextAligner(database, 3);
    }

    @Test(invocationTimeOut = 10000, invocationCount = 1, enabled = false)
    public void alignShortSequence() {
        List<String> query = database.subList(100, 200);
        Integer[] ids = new Integer[query.size()];
        for (int i = 0; i < query.size(); ++i)
            ids[i] = 100 + i;
        assertThat(Ints.asList(aligner.align(query)), contains(ids));
    }

    @Test(invocationTimeOut = 10000, invocationCount = 1, enabled = false)
    public void alignLongSequence() {
        List<String> query = database.subList(1999, 8777);
        assertThat(Ints.asList(aligner.align(query)), contains(1));
    }
}
