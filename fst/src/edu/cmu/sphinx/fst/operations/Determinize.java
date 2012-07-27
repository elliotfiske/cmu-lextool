/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.ArrayList;
import java.util.Iterator;

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
		Pair<State<T>, Weight<T>> p;
		State<T> s;
		Arc<T> arc;
		for(int i=0; i<pa.size(); i++) {
			p = pa.get(i);
			s = p.getLeft();
				
			for(Iterator<Arc<T>> itA = s.arcIterator(); itA.hasNext();) {
				arc = itA.next();
				if(!res.contains(arc.getIlabel())) {
					res.add(arc.getIlabel());
				}
			}
		}
		return res;
	}
	
	private static <T extends Comparable<T>> String getStateLabel(ArrayList<Pair<State<T>, Weight<T>>> pa) {
		StringBuilder sb = new StringBuilder();
		
		Pair<State<T>, Weight<T>> p;
		for(int i=0; i<pa.size(); i++) {
			p = pa.get(i);
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
		
		ArrayList<Pair<State<T>, Weight<T>>> p;
		State<T> pnew;
		Weight<T> wnew;
		Pair<State<T>, Weight<T>> ps;
		State<T> old;
		Weight<T> u;
		Arc<T> arc;
		Weight<T> wnewRevert;
		State<T> oldstate;
		Pair<State<T>, Weight<T>> pair;
		Weight<T> unew;
		Weight<T> fw;
		
		while(queue.size() > 0) {
			p = queue.get(0); 
			pnew = res.getStateById(getStateLabel(p));
			queue.remove(0);
			ArrayList<Integer> labels = getUniqueLabels(fst, p);
			for(int j=0; j <labels.size(); j++) {
				wnew = semiring.zero();
				// calc w'
				for(int k=0; k<p.size(); k++) {
					ps = p.get(k);
					old = ps.getLeft(); 
					u = p.get(k).getRight();
					for(Iterator<Arc<T>> itA = old.arcIterator(); itA.hasNext();) {
						arc = itA.next();
						if(labels.get(j).equals(arc.getIlabel())) {
							wnew = semiring.plus(wnew, semiring.times(u, arc.getWeight()));
						}
					}
				}
				
				// calc new states 
				// keep residual weights to variable forQueue 
				ArrayList<Pair<State<T>, Weight<T>>> forQueue = new ArrayList<Pair<State<T>, Weight<T>>>();
				for(int k=0; k<p.size(); k++) {
					ps = p.get(k);
					old = ps.getLeft(); 
					u = p.get(k).getRight();
					wnewRevert = semiring.divide(semiring.one(), wnew);
					for(Iterator<Arc<T>> itA = old.arcIterator(); itA.hasNext();) {
						arc = itA.next();
						if(labels.get(j).equals(arc.getIlabel())) {
							oldstate = fst.getStateById(arc.getNextStateId());
							pair = getPair(forQueue, oldstate, semiring.zero());
							pair.setRight(semiring.plus(pair.getRight(), semiring.times(wnewRevert, semiring.times(u, arc.getWeight()))));
						}
					}
				}
				
				// build new state's id and new elements for queue 
				String qnewid = "";
				for(int i=0; i<forQueue.size(); i++) {
					old = forQueue.get(i).getLeft();
					unew = forQueue.get(i).getRight();
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
					fw = qnew.getFinalWeight();
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
