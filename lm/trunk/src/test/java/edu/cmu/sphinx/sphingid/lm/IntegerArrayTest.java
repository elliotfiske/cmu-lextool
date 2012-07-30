package edu.cmu.sphinx.sphingid.lm;

import static org.hamcrest.Matchers.*;
import static org.junit.Assert.assertThat;
import org.junit.Test;

public class IntegerArrayTest {
	@Test
	public void canShuffle() {
		IntegerArray intArray = new IntegerArray(100);
		IntegerArray origArray = new IntegerArray(100);
		for (int i = 0; i < 100; ++i) {
			intArray.put(i, i);
			origArray.put(i, i);
		}
		intArray.shuffle();

		for (int times = 0; times < 20; ++times) {
			if (origArray.equals(intArray))
				intArray.shuffle();
		}

		assertThat(origArray, not(intArray));

	}
}	
