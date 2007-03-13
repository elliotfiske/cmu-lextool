package edu.cmu.sphinx.frontend.util;

import java.io.File;

/**
 * An interface which is describes the functionality which is required to handle new file signals fired by the
 * aduio-data sources.
 *
 * @author Holger Brandl
 */

public interface NewFileListener {

    /**
     * This method is invoked whenever a new file is started to become processed by a audio file data source.
     *
     * @param audioFile The name of the new audio file
     */
    public void newFileProcessingStarted(File audioFile);
}
