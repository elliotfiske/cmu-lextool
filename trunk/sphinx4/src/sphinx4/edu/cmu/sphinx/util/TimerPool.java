package edu.cmu.sphinx.util;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.WeakHashMap;

/**
 * Keeps reference to a list of timers.
 *
 * @author Holger Brandl
 */
public class TimerPool {

    private static final Map<Object, List<Timer>> weakRefTimerPool = new WeakHashMap<Object, List<Timer>>();


    // disable the constructor
    private TimerPool() {

    }

    /**
     * Retrieves (or creates) a timer with the given name
     *
     * @param owner
     * @param timerName the name of the particular timer to retrieve. If the timer does not already exist, it will be
     *                  created  @return the timer.
     */
    public static Timer getTimer(Object owner, String timerName) {
        if (!weakRefTimerPool.containsKey(owner))
            weakRefTimerPool.put(owner, new ArrayList<Timer>());

        List<Timer> ownerTimers = weakRefTimerPool.get(owner);

        for (Timer timer : ownerTimers) {
            if (timer.getName().equals(timerName))
                return timer;
        }

        // there is no timer named 'timerName' yet, so create it
        Timer requestedTimer = new Timer(timerName);
        ownerTimers.add(requestedTimer);

        return requestedTimer;
    }


    /** Dump all timers */
    public static void dumpAll() {
        showTimesShortTitle();

        for (Object owner : weakRefTimerPool.keySet()) {
            for (Timer timer : weakRefTimerPool.get(owner)) {
                timer.dump();
            }
        }
    }


    /** Shows the timing stats title. */
    private static void showTimesShortTitle() {
        String title = "Timers";
        String titleBar =
                "# ----------------------------- " + title +
                        "----------------------------------------------------------- ";
        System.out.println(Utilities.pad(titleBar, 78));
        System.out.print(Utilities.pad("# Name", 15) + " ");
        System.out.print(Utilities.pad("Count", 8));
        System.out.print(Utilities.pad("CurTime", 10));
        System.out.print(Utilities.pad("MinTime", 10));
        System.out.print(Utilities.pad("MaxTime", 10));
        System.out.print(Utilities.pad("AvgTime", 10));
        System.out.print(Utilities.pad("TotTime", 10));
        System.out.println();
    }


    /** Resets all timers */
    public static void resetAll() {
        for (Object owner : weakRefTimerPool.keySet()) {
            for (Timer timer : weakRefTimerPool.get(owner)) {
                timer.reset();
            }
        }
    }
}
