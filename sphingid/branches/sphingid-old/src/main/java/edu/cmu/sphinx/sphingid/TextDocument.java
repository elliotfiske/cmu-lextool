package edu.cmu.sphinx.sphingid;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;

/**
 * A class for containing an article extracted from a Nutch dump.
 * 
 * @author Emre Çelikten
 */
class TextDocument {
	private File file;
	private String topic, name, url;

	/**
	 * Creates a TextDocument object from file. Reads the topic from the file
	 * itself and gets its corresponding URL from the hash table. This
	 * constructor is not intended for batch use. Iterate over hashfile and use
	 * the other constructor in such a case.
	 * 
	 * @param file
	 *            The file that this object will contain.
	 * @param hashfile
	 *            Hash table that contains filenames and their associated URLs.
	 * 
	 * @throws IOException
	 *             if there is a problem with reading files
	 * @throws IllegalArgumentException
	 *             if one if the files do not exist
	 */
	public TextDocument(File file, File hashfile,
			boolean isStartEndAddedDocument) throws IOException,
			IllegalArgumentException {
		if (!file.canRead() || !hashfile.canRead()) {
			throw new IllegalArgumentException(
					"File "
							+ file.getName()
							+ " cannot be read. Check if the file exists or permissions are correctly specified.");
		}
		BufferedReader br = new BufferedReader(new FileReader(file));
		if (isStartEndAddedDocument) {
			br.readLine();
			this.topic = br.readLine().replaceAll("</?s>", "");
		} else
			this.topic = br.readLine();

		br.close();

		br = new BufferedReader(new FileReader(hashfile));

		String temp;
		while (!(temp = br.readLine()).startsWith(file.getName()))
			;
		this.url = temp.replace(file.getName() + " ", "");

		br.close();

	}

	public TextDocument(File file, String topic, String url)
			throws FileNotFoundException {
		if (!file.canRead()) {
			throw new FileNotFoundException(
					"File "
							+ file.getName()
							+ " cannot be read. Check if the file exists or permissions are correctly specified.");
		}

		this.file = file;
		this.name = file.getName();
		this.topic = topic;
		this.url = url;
	}

	/**
	 * Gets the file object for this text document.
	 * 
	 * @return The file.
	 */
	public File getFile() {
		return this.file;
	}

	/**
	 * Gets the topic for this text document.
	 * 
	 * @return The topic.
	 */
	public String getTopic() {

		return this.topic;
	}

	/**
	 * Gets the file name for this document.
	 * 
	 * @return The name.
	 */
	public String getName() {
		return this.name;
	}

	/**
	 * Gets the url for this text document.
	 * 
	 * @return The url.
	 */
	public String getUrl() {
		return this.url;
	}

	/**
	 * Reads the document file and returns its content as a string. Does not
	 * include its topic.
	 * 
	 * {@inheritDoc}
	 * 
	 * @see Object#toString()
	 */
	public String toString() {
		StringBuilder builder = new StringBuilder(10000);

		builder.append("</s>\n");

		BufferedReader br = null;
		try {
			br = new BufferedReader(new FileReader(this.getFile()));

			// The file should always exist as we have checked it before. This
			// one is just for the compiler.
		} catch (FileNotFoundException e) {
			e.printStackTrace();
			return null;
		}

		String temp = "";

		try {
			if (!br.ready())
				throw new IllegalArgumentException("Cannot read file:"
						+ this.getName());
			while (!temp.equals("</s>") && br.ready())
				temp = br.readLine();
			if (br.ready())
				br.readLine(); // Read topic and discard it
			while (br.ready()) {
				temp = br.readLine();
				builder.append(temp + "\n");

			}
			br.close();
		} catch (IOException e) {
			e.printStackTrace();
			return null;
		}

		builder.delete(builder.lastIndexOf("<s>"), builder.length());
		builder.append("<s>\n");

		return builder.toString();
	}
}
