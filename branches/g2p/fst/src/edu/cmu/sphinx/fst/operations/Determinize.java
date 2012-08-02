/**
 * 
 */
package edu.cmu.sphinx.fst.operations;

import java.util.ArrayList;
import java.util.HashMap;

import edu.cmu.sphinx.fst.Arc;
import edu.cmu.sphinx.fst.Fst;
import edu.cmu.sphinx.fst.State;
import edu.cmu.sphinx.fst.semiring.Semiring;
import edu.cmu.sphinx.fst.utils.Pair;

/**
 * @author John Salatas <jsalatas@users.sourceforge.net>
 * 
 */
public class Determinize {

    private static Pair<State, Float> getPair(
            ArrayList<Pair<State, Float>> queue, State state, Float zero) {
        Pair<State, Float> res = null;
        for (Pair<State, Float> tmp : queue) {
            if (state.getId() == tmp.getLeft().getId()) {
                res = tmp;
                break;
            }
        }

        if (res == null) {
            res = new Pair<State, Float>(state, zero);
            queue.add(res);
        }

        return res;
    }

    private static ArrayList<Integer> getUniqueLabels(Fst fst,
            ArrayList<Pair<State, Float>> pa) {
        ArrayList<Integer> res = new ArrayList<Integer>();

        for (Pair<State, Float> p : pa) {
            State s = p.getLeft();

            for (Arc arc : s.getArcs()) {
                if (!res.contains(arc.getIlabel())) {
                    res.add(arc.getIlabel());
                }
            }
        }
        return res;
    }

    private static Integer getStateLabel(ArrayList<Pair<State, Float>> pa,
            HashMap<String, Integer> stateMapper) {
        StringBuilder sb = new StringBuilder();

        for (Pair<State, Float> p : pa) {
            if (sb.length() > 0) {
                sb.append(",");
            }
            sb.append("(" + p.getLeft().getId() + "," + p.getRight() + ")");
        }
        return stateMapper.get(sb.toString());
    }

    public static Fst get(Fst fst) {

        if (fst.getSemiring() == null) {
            // semiring not provided
            return null;
        }

        // initialize the queue and new fst
        Semiring semiring = fst.getSemiring();
        Fst res = new Fst(semiring);
        res.setIsyms(fst.getIsyms());
        res.setOsyms(fst.getOsyms());

        // stores the queue (item in index 0 is next)
        ArrayList<ArrayList<Pair<State, Float>>> queue = new ArrayList<ArrayList<Pair<State, Float>>>();

        HashMap<String, Integer> stateMapper = new HashMap<String, Integer>();

        State s = new State(semiring.zero());
        String stateString = "(" + fst.getStart().getId() + ","
                + semiring.one() + ")";
        queue.add(new ArrayList<Pair<State, Float>>());
        queue.get(0)
                .add(new Pair<State, Float>(fst.getStart(), semiring.one()));
        res.addState(s);
        stateMapper.put(stateString, s.getId());
        res.setStart(s.getId());

        while (queue.size() > 0) {
            ArrayList<Pair<State, Float>> p = queue.get(0);
            State pnew = res.getStateById(getStateLabel(p, stateMapper));
            queue.remove(0);
            ArrayList<Integer> labels = getUniqueLabels(fst, p);
            for (int label : labels) {
                Float wnew = semiring.zero();
                // calc w'
                for (Pair<State, Float> ps : p) {
                    State old = ps.getLeft();
                    Float u = ps.getRight();
                    for (Arc arc : old.getArcs()) {
                        if (label == arc.getIlabel()) {
                            wnew = semiring.plus(wnew,
                                    semiring.times(u, arc.getWeight()));
                        }
                    }
                }

                // calc new states
                // keep residual weights to variable forQueue
                ArrayList<Pair<State, Float>> forQueue = new ArrayList<Pair<State, Float>>();
                for (Pair<State, Float> ps : p) {
                    State old = ps.getLeft();
                    Float u = ps.getRight();
                    Float wnewRevert = semiring.divide(semiring.one(), wnew);
                    for (Arc arc : old.getArcs()) {
                        if (label == arc.getIlabel()) {
                            State oldstate = fst.getStateById(arc
                                    .getNextStateId());
                            Pair<State, Float> pair = getPair(forQueue,
                                    oldstate, semiring.zero());
                            pair.setRight(semiring.plus(
                                    pair.getRight(),
                                    semiring.times(wnewRevert,
                                            semiring.times(u, arc.getWeight()))));
                        }
                    }
                }

                // build new state's id and new elements for queue
                String qnewid = "";
                for (Pair<State, Float> ps : forQueue) {
                    State old = ps.getLeft();
                    Float unew = ps.getRight();
                    if (!qnewid.equals("")) {
                        qnewid = qnewid + ",";
                    }
                    qnewid = qnewid + "(" + old.getId() + "," + unew + ")";
                }

                if (stateMapper.get(qnewid) == null || res.getStateById(stateMapper.get(qnewid)) == null) {
                    State qnew = new State(semiring.zero());
                    res.addState(qnew);
                    stateMapper.put(qnewid, qnew.getId());
                    // update new state's weight
                    Float fw = qnew.getFinalWeight();
                    for (Pair<State, Float> ps : forQueue) {
                        fw = semiring.plus(fw, semiring.times(ps.getLeft()
                                .getFinalWeight(), ps.getRight()));
                    }
                    qnew.setFinalWeight(fw);

                    queue.add(forQueue);
                }
                pnew.addArc(new Arc(label, label, wnew, stateMapper
                        .get(qnewid)));
            }
        }

        return res;
    }
}
