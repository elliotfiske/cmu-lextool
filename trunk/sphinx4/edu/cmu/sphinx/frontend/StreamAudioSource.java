/**
 * [[[copyright]]]
 */

package edu.cmu.sphinx.frontend;

import edu.cmu.sphinx.util.SphinxProperties;
import java.io.IOException;
import java.io.InputStream;
import java.util.Vector;


/**
 * A StreamAudioSource converts data from an InputStream into
 * DoubleAudioFrame(s). One would obtain the DoubleAudioFrames using
 * the <code>read()</code> method. It will make sure that the
 * returned DoubleAudioFrame can be made into exactly N windows.
 * The size of the windows and the window shift is specified by
 * the SphinxProperties
 * <pre>
 * edu.cmu.sphinx.frontend.windowSize
 * edu.cmu.sphinx.frontend.windowShift
 * </pre>The audio samples that do not fit into the current frame
 * will be used in the next frame (which is obtained by the next call to
 * <code>read()</code>).
 */
public class StreamAudioSource implements DataSource {

    /**
     * The name of the SphinxProperty which indicates if the produced
     * DoubleAudioFrames should be dumped. The default value of this
     * SphinxProperty is false.
     */
    public static final String PROP_DUMP =
	"edu.cmu.sphinx.frontend.doubleAudioFrameSource.dump";

    private static final int SEGMENT_MAX_BYTES = 2000000;
    private static final int SEGMENT_NOT_STARTED = -1;

    private InputStream audioStream;

    private int frameSizeInBytes;
    private int windowSizeInBytes;
    private int windowShiftInBytes;

    /**
     * The buffer that contains the samples. "totalInBuffer" indicates
     * the number of bytes in the samplesBuffer so far.
     */
    private byte[] samplesBuffer;
    private int totalInBuffer;
    private int totalBytesRead;

    /**
     * The buffer that contains the overflow samples.
     */
    private byte[] overflowBuffer;
    private int overflowBytes;

    private Vector outputQueue;
    private boolean dump;


    /**
     * Constructs a StreamAudioSource with the given InputStream.
     *
     * @param audioStream the InputStream where audio data comes from
     */
    public StreamAudioSource(InputStream audioStream) {
	getSphinxProperties();

	this.audioStream = audioStream;
	this.samplesBuffer = new byte[frameSizeInBytes * 2];
        this.overflowBuffer = new byte[frameSizeInBytes];
        this.outputQueue = new Vector();
        reset();
    }


    /**
     * Resets this StreamAudioSource into a state ready to read the
     * current InputStream.
     */
    public void reset() {
        this.totalBytesRead = SEGMENT_NOT_STARTED;
        this.overflowBytes = 0;
        outputQueue.clear();
    }


    /**
     * Reads the parameters needed from the static SphinxProperties object.
     */
    private void getSphinxProperties() {
	// TODO : specify the context
	SphinxProperties properties = SphinxProperties.getSphinxProperties("");
	
        windowSizeInBytes = properties.getInt
            (FrontEnd.PROP_WINDOW_SIZE, 205) * 2;
	
        windowShiftInBytes = properties.getInt
	    (FrontEnd.PROP_WINDOW_SHIFT, 80) * 2;
	
        frameSizeInBytes = properties.getInt
	    (FrontEnd.PROP_BYTES_PER_AUDIO_FRAME, 4000);
	
        dump = properties.getBoolean(PROP_DUMP, false);
    }


    /**
     * Sets the InputStream from which this StreamAudioSource reads.
     *
     * @param inputStream the InputStream from which audio data comes
     */
    public void setInputStream(InputStream inputStream) {
	this.audioStream = inputStream;
    }

    
    /**
     * Reads and returns the next DoubleAudioFrame from the InputStream of
     * StreamAudioSource, return null if no data is read and end of file
     * is reached.
     *
     * @return the next DoubleAudioFrame or <code>null</code> if none is
     *     available
     *
     * @throws java.io.IOException
     */
    public Data read() throws IOException {

        Data output = null;
 
	if (!outputQueue.isEmpty()) {
	    
            return (Data) outputQueue.remove(0);

	} else if (totalBytesRead == SEGMENT_NOT_STARTED) {

            totalBytesRead = 0;
            return SegmentEndPointSignal.getStartSignal();

	} else if (totalBytesRead >= SEGMENT_MAX_BYTES) {

            totalBytesRead = SEGMENT_NOT_STARTED;
	    return SegmentEndPointSignal.getEndSignal();

	} else {

	    return readNextFrame();
	}
    }


    /**
     * Reads the next DoubleAudioFrame from the input stream.
     *
     * @return a DoubleAudioFrame
     *
     * @throws java.io.IOException
     */
    private DoubleAudioFrame readNextFrame() throws IOException {

	totalInBuffer = 0;

	// if there are previous samples, pre-pend them to input speech samps
        overflowToSamplesBuffer();

	// read one frame from the inputstream
	readInputStream(frameSizeInBytes);

	// if nothing in the samplesBuffer, return null
	if (totalInBuffer == 0) {
	    return null;
	}
	
	// if read bytes do not fill a frame, copy them to overflow buffer
	// after the previously stored overlap samples
	if (totalInBuffer < windowSizeInBytes) {

            samplesToOverflowBuffer(0, overflowBytes, totalInBuffer);
	    return null;

	} else {
	    int occupiedElements = Util.getOccupiedElements
		(totalInBuffer, windowSizeInBytes, windowShiftInBytes);

	    if (occupiedElements < totalInBuffer) {

		// assign samples which don't fill an entire frame to 
		// overflow buffer for use on next pass
		int offset = occupiedElements - 
                    (windowSizeInBytes - windowShiftInBytes);

                samplesToOverflowBuffer(offset, 0, (totalInBuffer - offset));

		totalInBuffer = occupiedElements;

		// set "prior" for the next read
		short prior = Util.bytesToShort(samplesBuffer, offset - 2);
		outputQueue.add(new PreemphasisPriorSignal(prior));
	    }

	    if (totalInBuffer % 2 == 1) {
		samplesBuffer[totalInBuffer++] = 0;
	    }

	    // convert the byte[] into a DoubleAudioFrame
	    double[] audioFrame = Util.byteToDoubleArray
		(samplesBuffer, 0, totalInBuffer);
            
	    if (dump) {
		Util.dumpDoubleArray(audioFrame, "FRAME_SOURCE");
	    }

	    return (new DoubleAudioFrame(audioFrame));
	}
    }


    /**
     * Copies the overflow samples in the overflowBuffer to the
     * beginning of the samplesBuffer.
     * Increments totalInBuffer, and sets overflowBytes to zero.
     */
    private void overflowToSamplesBuffer() {
	if (overflowBytes > 0) {
	    System.arraycopy(overflowBuffer, 0, samplesBuffer, 0,
			     overflowBytes);
	    totalInBuffer = overflowBytes;
	    overflowBytes = 0;
	}
    }


    /**
     * Copies from the samplesBuffer at the specified position, to
     * the overflowBuffer at the specified position.
     *
     * @param samplesPos where to start in the samplesBuffer
     * @param overflowPos where to start in the overflowBuffer
     * @param length how many bytes to copy
     */
    private void samplesToOverflowBuffer(int samplesPos, int overflowPos,
                                         int length) {
        if (totalInBuffer > 0) {
            System.arraycopy(samplesBuffer, samplesPos,
                             overflowBuffer, overflowPos,
                             length);
            overflowBytes = overflowPos + length;
        }
    }


    /**
     * Reads the specified number of bytes from the InputStream, and
     * return the number of bytes read.
     *
     * @param numberOfBytes the number of bytes to read
     *
     * @return the number of bytes read
     */
    private int readInputStream(int numberOfBytes) throws IOException {
	int read = 0;
	int totalRead = 0;
	do {
	    read = audioStream.read
		(samplesBuffer, totalInBuffer, numberOfBytes - totalRead);
	    if (read > 0) {
		totalRead += read;
		totalInBuffer += read;
	    }
	} while (read != -1 && totalRead < numberOfBytes);
	
	totalBytesRead += totalRead;

	return totalRead;
    }
}
