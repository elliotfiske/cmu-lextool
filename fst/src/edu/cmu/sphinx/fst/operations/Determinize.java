/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.ArrayList;
import java.util.Iterator;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Pair;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Determinize {
	
	private static Pair<State, Double> getPair(ArrayList<Pair<State, Double>> queue, State state, Double zero) {
		Pair<State, Double> res = null;
		Pair<State, Double> tmp;
		for(Iterator<Pair<State, Double>> it = queue.iterator(); it.hasNext();) {
			tmp = it.next();
			if(state.getId().equals(tmp.getLeft().getId())) {
				res = tmp;
				break;
			}
		}
		
		if(res==null) {
			res = new Pair<State, Double>(state, zero);
			queue.add(res);
		}
		
		return res;
	}
	private static ArrayList<Integer> getUniqueLabels(Fst fst, ArrayList<Pair<State, Double>> pa) {
		ArrayList<Integer> res = new ArrayList<Integer>();
		Pair<State, Double> p;
		State s;
		Arc arc;
		for(Iterator<Pair<State, Double>> it = pa.iterator(); it.hasNext();) {
			p = it.next();
			s = p.getLeft();
				
			for(Iterator<Arc> itA = s.arcIterator(); itA.hasNext();) {
				arc = itA.next();
				if(!res.contains(arc.getIlabel())) {
					res.add(arc.getIlabel());
				}
			}
		}
		return res;
	}
	
	private static String getStateLabel(ArrayList<Pair<State, Double>> pa) {
		StringBuilder sb = new StringBuilder();
		
		Pair<State, Double> p;
		for(Iterator<Pair<State, Double>> it = pa.iterator(); it.hasNext();) {
			p = it.next();
			if(sb.length()> 0) {
				sb.append(",");
			}
			sb.append("("+ p.getLeft().getId() + "," + p.getRight() + ")");
		}
		return sb.toString();
	}

	public static Fst get(Fst fst) {
		if (fst == null) {
			return null;
		}
		
		if(fst.getSemiring() == null) {
			// semiring not provided
			return null;
		}
		
		// initialize the queue and new fst
		Semiring semiring = fst.getSemiring();
		Fst res = new Fst(semiring);
		res.setIsyms(fst.getIsyms());
		res.setOsyms(fst.getOsyms());

		//stores the queue (item in index 0 is next)
		ArrayList<ArrayList<Pair<State, Double>>> queue = new ArrayList<ArrayList<Pair<State, Double>>>();

		State s = new State(semiring.zero());
		s.setId("("+fst.getStart().getId()+"," +semiring.one()+")");
		queue.add(new ArrayList<Pair<State, Double>>());
		queue.get(0).add(new Pair<State, Double>(fst.getStart(), semiring.one()));
		res.addState(s);
		res.setStart(s.getId());
		
		ArrayList<Pair<State, Double>> p;
		State pnew;
		Double wnew;
		Pair<State, Double> ps;
		State old;
		Double u;
		Arc arc;
		Double wnewRevert;
		State oldstate;
		Pair<State, Double> pair;
		Double unew;
		Double fw;
		int label;
		
		while(queue.size() > 0) {
			p = queue.get(0); 
			pnew = res.getStateById(getStateLabel(p));
			queue.remove(0);
			ArrayList<Integer> labels = getUniqueLabels(fst, p);
			for(Iterator<Integer> itL = labels.iterator(); itL.hasNext();) {
				label = itL.next().intValue();
				wnew = semiring.zero();
				// calc w'
				for(Iterator<Pair<State, Double>> itP = p.iterator(); itP.hasNext();) {
					ps = itP.next();
					old = ps.getLeft(); 
					u = ps.getRight();
					for(Iterator<Arc> itA = old.arcIterator(); itA.hasNext();) {
						arc = itA.next();
						if(label == arc.getIlabel()) {
							wnew = semiring.plus(wnew, semiring.times(u, arc.getWeight()));
						}
					}
				}
				
				// calc new states 
				// keep residual weights to variable forQueue 
				ArrayList<Pair<State, Double>> forQueue = new ArrayList<Pair<State, Double>>();
				for(Iterator<Pair<State, Double>> itP = p.iterator(); itP.hasNext();) {
					ps = itP.next();
					old = ps.getLeft(); 
					u = ps.getRight();
					wnewRevert = semiring.divide(semiring.one(), wnew);
					for(Iterator<Arc> itA = old.arcIterator(); itA.hasNext();) {
						arc = itA.next();
						if(label == arc.getIlabel()) {
							oldstate = fst.getStateById(arc.getNextStateId());
							pair = getPair(forQueue, oldstate, semiring.zero());
							pair.setRight(semiring.plus(pair.getRight(), semiring.times(wnewRevert, semiring.times(u, arc.getWeight()))));
						}
					}
				}
				
				// build new state's id and new elements for queue 
				String qnewid = "";
				for(Iterator<Pair<State, Double>> itQ = forQueue.iterator(); itQ.hasNext();) {
					ps = itQ.next();
					old = ps.getLeft();
					unew = ps.getRight();
					if (!qnewid.equals("")) {
						qnewid =qnewid +",";
					}
					qnewid = qnewid + "(" + old.getId() + "," + unew + ")";
				}
				
				pnew.addArc(new Arc(label, label, wnew, qnewid));
				if(res.getStateById(qnewid) == null) {
					State qnew = new State(semiring.zero());
					qnew.setId(qnewid);
					res.addState(qnew);
					// update new state's weight
					fw = qnew.getFinalWeight();
					for(Iterator<Pair<State, Double>> itQ = forQueue.iterator(); itQ.hasNext();) {
						ps = itQ.next();
						fw = semiring.plus(fw, semiring.times(ps.getLeft().getFinalWeight(), ps.getRight()));
					}
					qnew.setFinalWeight(fw);
					
					queue.add(forQueue);
				}
			}
		}
		
		return res;
	}
}
