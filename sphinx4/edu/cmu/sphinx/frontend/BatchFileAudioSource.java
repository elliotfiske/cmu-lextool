/**
 * [[[copyright]]]
 */

package edu.cmu.sphinx.frontend;

import java.io.BufferedReader;
import java.io.FileInputStream;
import java.io.FileReader;
import java.io.IOException;


/**
 * A BatchFileAudioSource takes a file (called batch file onwards)
 * that contains a list of audio files,
 * and converts the audio data in each of the audio files into
 * Audio(s). One would obtain the Audios using
 * the <code>read()</code> method. This class uses the StreamAudioSource
 * class. In fact, it converts each audio file in the batch file into
 * an InputStream, and sets it to the InputStream of StreamAudioSource.
 * Its read() method then calls StreamAudioSource.read().
 * The only difference is that BatchFileAudiosource
 * takes a batch file, whereas StreamAudioSource takes an InputStream.
 *
 * The format of the batch file would look like: <pre>
 * /home/user1/data/music.au
 * /home/user1/data/talking.au
 * ...
 * </pre>
 *
 * @see StreamAudioSource
 */
public class BatchFileAudioSource extends DataProcessor implements
AudioSource {

    private BufferedReader reader;
    private StreamAudioSource streamAudioSource = null;


    /**
     * Constructs a BatchFileAudioSource with the given InputStream.
     *
     * @param batchFile contains a list of the audio files
     *
     * @throws java.io.IOException if error opening the batch file
     */
    public BatchFileAudioSource(String name, String context,
                                String batchFile) throws
    IOException {
        super(name, context);
        reader = new BufferedReader(new FileReader(batchFile));
        streamAudioSource = new StreamAudioSource
            ("StreamAudioSource", context, null);

        String firstFile = reader.readLine();
        if (firstFile != null) {
            fileSetStream(firstFile);
        }
    }


    /**
     * Construct an InputStream with the given audio file, and set it as
     * the InputStream of the streamAudioSource.
     *
     * @param audioFile the file containing audio data
     *
     * @throws java.io.IOException if error setting the stream
     */
    private void fileSetStream(String audioFile) throws IOException {
        streamAudioSource.setInputStream
            (new FileInputStream(audioFile + ".raw"));
    }

    
    /**
     * Reads and returns the next Audio. 
     * Returns null if all the data in all the files have been read.
     *
     * @return the next Audio or <code>null</code> if no more is
     *     available
     *
     * @throws java.io.IOException
     */
    public Audio getAudio() throws IOException {
        if (streamAudioSource == null) {
            return null;
        }
        
        Audio frame = streamAudioSource.getAudio();
        if (frame != null) {
            return frame;
        } else {

            // assumes that the reader has already been opened
            if (reader != null) {
                // if we reached the end of the current file,
                // go to the next one
                String nextFile = reader.readLine();
                if (nextFile != null) {
                    fileSetStream(nextFile);
                    return getAudio();
                } else {
                    reader.close();
                    reader = null;
                }
            }
        }

        return null;
    }
}



