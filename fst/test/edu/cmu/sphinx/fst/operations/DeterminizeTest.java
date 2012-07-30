/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import static org.junit.Assert.assertTrue;

import org.junit.Test;

import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.openfst.Convert;
import edu.cmu.sphinx.fst.semiring.TropicalSemiring;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class DeterminizeTest {
    @Test
    public void testDeterminize() {
        System.out.println("Testing Determinization...");
        Fst fstA = Convert.importFloat("data/tests/algorithms/determinize/A",
                new TropicalSemiring());
        Fst determinized = Convert.importFloat(
                "data/tests/algorithms/determinize/fstdeterminize",
                new TropicalSemiring());

        Fst fstDeterminized = Determinize.get(fstA);
        assertTrue(determinized.equals(fstDeterminized));

        System.out.println("Testing Determinization Completed!\n");
    }

    public static void main(String[] args) {
        DeterminizeTest test = new DeterminizeTest();
        test.testDeterminize();
    }
}
