/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.ArrayList;

import edu.cmu.sphinx.fst.arc.Arc;
import edu.cmu.sphinx.fst.fst.Fst;
import edu.cmu.sphinx.fst.state.State;
import edu.cmu.sphinx.fst.utils.Pair;
import edu.cmu.sphinx.fst.weight.Semiring;
import edu.cmu.sphinx.fst.weight.Weight;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 *
 */
public class Determinize {
	
	private static <T extends Comparable<T>> Pair<State<T>, Weight<T>> getPair(ArrayList<Pair<State<T>, Weight<T>>> queue, State<T> state, Weight<T> zero) {
		Pair<State<T>, Weight<T>> res = null;
		for(int i=0;i<queue.size();i++) {
			if(state.getId().equals(queue.get(i).getLeft().getId())) {
				res = queue.get(i);
				break;
			}
		}
		
		if(res==null) {
			res = new Pair<State<T>, Weight<T>>(state, zero);
			queue.add(res);
		}
		
		return res;
	}
	private static <T extends Comparable<T>> ArrayList<Integer> getUniqueLabels(Fst<T> fst, ArrayList<Pair<State<T>, Weight<T>>> pa) {
		ArrayList<Integer> res = new ArrayList<Integer>();
		for(int i=0; i<pa.size(); i++) {
			Pair<State<T>, Weight<T>> p = pa.get(i);
				State<T> s = p.getLeft();
				
				for(int j=0; j< s.getNumArcs();j++) {
					Arc<T> arc = s.getArc(j);
					if(!res.contains(arc.getIlabel())) {
						res.add(arc.getIlabel());
					}
				}
		}
		return res;
	}
	
	private static <T extends Comparable<T>> String getStateLabel(ArrayList<Pair<State<T>, Weight<T>>> pa) {
		StringBuilder sb = new StringBuilder();
		
		for(int i=0; i<pa.size(); i++) {
			Pair<State<T>, Weight<T>> p = pa.get(i);
			if(sb.length()> 0) {
				sb.append(",");
			}
			sb.append("("+ p.getLeft().getId() + "," + p.getRight() + ")");
		}
		return sb.toString();
	}

	public static <T extends Comparable<T>> Fst<T> get(Fst<T> fst) {
		if (fst == null) {
			return null;
		}
		
		if(fst.getSemiring() == null) {
			// semiring not provided
			return null;
		}
		
		// initialize the queue and new fst
		Semiring<T> semiring = fst.getSemiring();
		Fst<T> res = new Fst<T>(semiring);
		res.setIsyms(fst.getIsyms());
		res.setOsyms(fst.getOsyms());

		//stores the queue (item in index 0 is next)
		ArrayList<ArrayList<Pair<State<T>, Weight<T>>>> queue = new ArrayList<ArrayList<Pair<State<T>, Weight<T>>>>();

		State<T> s = new State<T>(semiring.zero());
		s.setId("("+fst.getStart().getId()+"," +semiring.one()+")");
		queue.add(new ArrayList<Pair<State<T>, Weight<T>>>());
		queue.get(0).add(new Pair<State<T>, Weight<T>>(fst.getStart(), semiring.one()));
		res.addState(s);
		res.setStart(s.getId());
		
		while(queue.size() > 0) {
			ArrayList<Pair<State<T>, Weight<T>>> p = queue.get(0); 
			State<T> pnew = res.getStateById(getStateLabel(p));
			queue.remove(0);
			ArrayList<Integer> labels = getUniqueLabels(fst, p);
			for(int j=0; j <labels.size(); j++) {
				Weight<T> wnew = semiring.zero();
				// calc w'
				for(int k=0; k<p.size(); k++) {
					Pair<State<T>, Weight<T>> ps = p.get(k);
					State<T> old = ps.getLeft(); 
					Weight<T> u = p.get(k).getRight();
					for (int i=0;i<old.getNumArcs(); i++) {
						Arc<T> arc = old.getArc(i);
						if(labels.get(j).equals(arc.getIlabel())) {
							wnew = semiring.plus(wnew, semiring.times(u, arc.getWeight()));
						}
					}
				}
				
				// calc new states 
				// keep residual weights to variable forQueue 
				ArrayList<Pair<State<T>, Weight<T>>> forQueue = new ArrayList<Pair<State<T>, Weight<T>>>();
				for(int k=0; k<p.size(); k++) {
					Pair<State<T>, Weight<T>> ps = p.get(k);
					State<T> old = ps.getLeft(); 
					Weight<T> u = p.get(k).getRight();
					Weight<T> wnewRevert = semiring.divide(semiring.one(), wnew);
					for (int i=0;i<old.getNumArcs(); i++) {
						Arc<T> arc = old.getArc(i);
						if(labels.get(j).equals(arc.getIlabel())) {
							State<T> oldstate = fst.getStateById(arc.getNextStateId());
							Pair<State<T>, Weight<T>> pair = getPair(forQueue, oldstate, semiring.zero());
							pair.setRight(semiring.plus(pair.getRight(), semiring.times(wnewRevert, semiring.times(u, arc.getWeight()))));
						}
					}
				}
				
				// build new state's id and new elements for queue 
				String qnewid = "";
				for(int i=0; i<forQueue.size(); i++) {
					State<T> old = forQueue.get(i).getLeft();
					Weight<T> unew = forQueue.get(i).getRight();
					if (!qnewid.equals("")) {
						qnewid =qnewid +",";
					}
					qnewid = qnewid + "(" + old.getId() + "," + unew + ")";
				}
				
				pnew.addArc(new Arc<T>(labels.get(j), labels.get(j), wnew, qnewid));
				if(res.getStateById(qnewid) == null) {
					State<T> qnew = new State<T>(semiring.zero());
					qnew.setId(qnewid);
					res.addState(qnew);
					// update new state's weight
					Weight<T> fw = qnew.getFinalWeight();
					for(int i=0; i<forQueue.size(); i++) {
						Pair<State<T>, Weight<T>> fp = forQueue.get(i);
						fw = semiring.plus(fw, semiring.times(fp.getLeft().getFinalWeight(), fp.getRight()));
					}
					qnew.setFinalWeight(fw);
					
					queue.add(forQueue);
				}
			}
		}
		
		return res;
	}
}
