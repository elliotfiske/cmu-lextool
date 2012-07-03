package edu.cmu.sphinx.fst;

import org.junit.runner.JUnitCore;
import org.junit.runner.Result;
import org.junit.runner.RunWith;
import org.junit.runner.notification.Failure;
import org.junit.runners.Suite;
import org.junit.runners.Suite.SuiteClasses;

import edu.cmu.sphinx.fst.openfst.ImportTest;
import edu.cmu.sphinx.fst.operations.OperationsTest;

@RunWith(Suite.class)
@SuiteClasses({ImportTest.class,
	OperationsTest.class})
public class AllTests {

	/**
	 * for running the tests from commandline
	 * 
	 * @param args        the commandline arguments - ignored
	 */
	public static void main(String []args) {
		Result result = JUnitCore.runClasses(AllTests.class);
		for (Failure failure : result.getFailures()) {
			System.out.println(failure.toString());
		}
	}

}

