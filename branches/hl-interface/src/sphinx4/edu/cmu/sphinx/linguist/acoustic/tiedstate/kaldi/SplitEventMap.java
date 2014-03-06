package edu.cmu.sphinx.linguist.acoustic.tiedstate.kaldi;

import java.util.Collection;
import java.util.HashSet;
import java.util.Set;

import edu.cmu.sphinx.linguist.acoustic.Unit;


/**
 * Binary decision tree.
 *
 * Splits on a certain key and goes to the "yes" or "no" child node depending
 * on the answer. Its Map function calls the Map function of the appropriate
 * child node. It stores a set of integers of type kAnswerType that correspond
 * to the "yes" child (everything else goes to "no").
 */
public class SplitEventMap extends EventMapWithKey {

    private final Set<Integer> answers;
    private final EventMap yesMap;
    private final EventMap noMap;

    /**
     * Constructs new event map.
     *
     * @param key     key to split on
     * @param answers yes answers
     * @param yesMap  event map for "yes" answer
     * @param noMap   eventMap for no answer
     */
    public SplitEventMap(int key,
                         Collection<Integer> answers,
                         EventMap yesMap, EventMap noMap)
    {
        super(key);
        this.answers = new HashSet<Integer>(answers);
        this.yesMap = yesMap;
        this.noMap = noMap;
    }

    /**
     * Maps speech unit to probability distribution function.
     *
     * @param unit unit of speech.
     *
     * @return identifier of probability distribution function.
     */
    public int map(int pdfClass, Unit unit) {
        int id = (-1 == key ? pdfClass : getKeyValue(key, unit));
        return answers.contains(id) ?
               yesMap.map(pdfClass, unit) : noMap.map(pdfClass, unit);
    }
}

