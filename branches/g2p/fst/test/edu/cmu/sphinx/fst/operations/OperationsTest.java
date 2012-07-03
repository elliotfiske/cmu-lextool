package edu.cmu.sphinx.fst.operations;

import static org.junit.Assert.*;

import org.junit.Test;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.weight.Weight;

public class OperationsTest {

	/**
	 * Create an output label sorted fst
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ArcSortDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createOsorted() {
		Fst<Double> fst = new Fst<Double>();
		// State 0
		State<Double> s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 4, 1, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 5, 2, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 2, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 1, 4, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 3, 5, 1));
		fst.AddState(s);

		// State 1
		s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 3, 1, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 1, 2, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 2, 3, 1));
		fst.AddState(s);

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(0.));
		fst.AddState(s);
		
		return fst;
	}
	
	/**
	 * Create an input label sorted fst
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ArcSortDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createIsorted() {
		Fst<Double> fst = new Fst<Double>();
		// State 0
		State<Double> s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 1, 4, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 2, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 3, 5, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 4, 1, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 5, 2, 2));
		fst.AddState(s);

		// State 1
		s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 1, 2, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 2, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 3, 1, 2));
		fst.AddState(s);

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(0.));
		fst.AddState(s);
		
		return fst;
	}

	/**
	 * Create an unsorted fst
	 * as per the example at http://www.openfst.org/twiki/bin/view/FST/ArcSortDoc
	 * 
	 * @return the created fst
	 */
	private Fst<Double> createUnsorted() {
		Fst<Double> fst = new Fst<Double>();
		// State 0
		State<Double> s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 1, 4, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 3, 5, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 2, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 5, 2, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 4, 1, 2));
		fst.AddState(s);

		// State 1
		s = new State<Double>(); 
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 2, 3, 1));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 3, 1, 2));
		s.AddArc(new Arc<Double>(new Weight<Double>(0.), 1, 2, 2));
		fst.AddState(s);

		// State 2 (final)
		s = new State<Double>(new Weight<Double>(0.));
		fst.AddState(s);

		return fst;
	}
	@Test
	public void testArcSort() {
		System.out.println("Testing Arc Sort...");
		// Input label sort test
		Fst<Double> fst1 = createUnsorted();
		Fst<Double> fst2 = createIsorted();
		assertTrue(!fst1.equals(fst2));
		Operations.ArcSort(fst1, new ILabelCompare<Double>());
		assertTrue(fst1.equals(fst2));
		
		// Output label sort test
		fst1 = createUnsorted();
		fst2 = createOsorted();
		assertTrue(!fst1.equals(fst2));
		Operations.ArcSort(fst1, new OLabelCompare<Double>());
		assertTrue(fst1.equals(fst2));
	}

}
