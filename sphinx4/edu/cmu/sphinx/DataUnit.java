/**
 * [[[copyright]]]
 */

package edu.cmu.sphinx;

import java.io.Serializable;

/**
 * Represents a basic data frame. It is the data fed into a processor,
 * as well as the data generated by a processor, which is usually fed
 * into the next processor. Examples of a DataUnit include (but are
 * not limited to) a frame of audio data, pre-emphasized audio data,
 * or a feature vector.
 *
 * It is important for a speech recognizer to know if the current data
 * frame is the first or last of a data segment (a data segment is made
 * up of a series of DataUnits (or data frames)). The isSegmentStart()
 * and isSegmentEnd() methods provides that information. They are the
 * primary motivation for having this interface. If there is only one
 * DataUnit in the data segment, then both isSegmentStart() and
 * isSegmentEnd() return true.
 *
 */
public class DataUnit implements Serializable {

    private boolean segmentStart = false;
    private boolean segmentEnd = false;


    /**
     * Constructs a default DataUnit that is neither at the start of 
     * the data segment nor the end of the data segment.
     */
    public DataUnit() {}


    /**
     * Constructs a DataUnit and specify if it is at the start of the
     * data segment and/or the end of the data segment.
     *
     * @param segmentStart true if this DataUnit is the start of a segment,
     *                     false otherwise
     *
     * @param segmentEnd true if this DataUnit is the end of a segment,
     *                   false otherwise 
     */
    public DataUnit(boolean segmentStart, boolean segmentEnd) {
	this.segmentStart = segmentStart;
	this.segmentEnd = segmentEnd;
    }


    /**
     * Returns true if this DataUnit is the first one in the data segment;
     * false otherwise.
     *
     * @return true if  this is the first DataUnit in the segment
     *         false otherwise
     */
    public boolean isSegmentStart() {
	return segmentStart;
    }


    /**
     * Returns true if this DataUnit is the last one in the data segment;
     * false otherwise.
     *
     * @return true if this is the last DataUnit in the segment
     *         false otherwise
     */
    public boolean isSegmentEnd() {
	return segmentEnd;
    }
}
