package edu.cmu.sphinx.fst.openfst;

import static org.junit.Assert.*;

import org.junit.Test;

import edu.cmu.sphinx.fst.fst.Fst;

public class ImportTest {

	@Test
	public void testConvert() {
		Fst<Double> fst1 = Import.convert("data/openfst/basic.fst.txt");
		
		@SuppressWarnings("unchecked")
		Fst<Double> fst2 = (Fst<Double>) Fst.loadModel("data/openfst/basic.fst.ser");
		
		assertEquals(fst2, fst1);
	}

}
