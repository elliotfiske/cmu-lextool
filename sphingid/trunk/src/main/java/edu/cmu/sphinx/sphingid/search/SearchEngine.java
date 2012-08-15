/**
 * 
 */
package edu.cmu.sphinx.sphingid.search;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.nio.charset.Charset;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.Field.Index;
import org.apache.lucene.document.Field.Store;
import org.apache.lucene.index.CorruptIndexException;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.queryParser.ParseException;
import org.apache.lucene.queryParser.QueryParser;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TopScoreDocCollector;
import org.apache.lucene.store.Directory;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;

/**
 * Class for interfacing to Apache Lucene. 
 * 
 * @author Emre Çelikten <emrecelikten@users.sourceforge.net>
 *
 */
public class SearchEngine {
	private static Analyzer analyzer = new StandardAnalyzer(Version.LUCENE_36);

	/**
	 * Returns a field from given document. 
	 * @param index search engine index
	 * @param field field to be read
	 * @param document document of which field will be retrieved
	 * @return field data as a String object
	 * @throws CorruptIndexException
	 * @throws IOException
	 */
	public static String getField(Directory index, ScoreDoc document, String field) throws CorruptIndexException, IOException {
		IndexReader reader = IndexReader.open(index);
		IndexSearcher searcher = new IndexSearcher(reader);
		Document d = searcher.doc(document.doc);
		searcher.close();
		return d.get(field);
	}
	
	/**
	 * Creates a search engine index from a given directory. Parses each file in the directory to construct the index.
	 * @param textDirectory directory of which files will be parsed
	 * @param encoding encoding to use while reading files
	 * @throws IOException when files cannot be read or index cannot be written
	 */
	public static void extractIndex(File textDirectory, Charset encoding)
			throws IOException {
		Directory fsDir = FSDirectory.open(textDirectory);
		IndexWriterConfig iwConf = new IndexWriterConfig(Version.LUCENE_36,
				analyzer);
		iwConf.setOpenMode(IndexWriterConfig.OpenMode.CREATE);
		IndexWriter indexWriter = new IndexWriter(fsDir, iwConf);

		for (File file : textDirectory.listFiles()) {
			BufferedReader br = new BufferedReader(new InputStreamReader(
					new FileInputStream(file), encoding));
			StringBuilder text = new StringBuilder();
			String temp = null;
			while ((temp = br.readLine()) != null) {
				text.append(temp);
			}

			Document document = new Document();
			document.add(new Field("filename", file.getName(), Store.YES,
					Index.NOT_ANALYZED));
			document.add(new Field("text", text.toString(), Store.NO,
					Index.ANALYZED));
			indexWriter.addDocument(document);
		}
		indexWriter.close();
	}
	
	/**
	 * Searches for a query in the given index.
	 * @param index index to be processed
	 * @param query search query
	 * @param numHits number of maximum documents that will be returned as results of the search
	 * @return documents as {@link ScoreDoc}s
	 * @throws ParseException
	 * @throws CorruptIndexException
	 * @throws IOException
	 */
	public static ScoreDoc[] query(Directory index, String query, int numHits) throws ParseException, CorruptIndexException, IOException {
		Query q = new QueryParser(Version.LUCENE_36, "text", analyzer)
				.parse(query);
		IndexReader reader = IndexReader.open(index);
		IndexSearcher searcher = new IndexSearcher(reader);
		TopScoreDocCollector collector = TopScoreDocCollector.create(numHits, true);
		searcher.search(q, collector);
		ScoreDoc results[] = collector.topDocs().scoreDocs;
		
		searcher.close();
		
		return results;
	}

	
}
