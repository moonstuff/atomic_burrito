package atomicburrito.indexsites;

import java.io.BufferedReader;
import java.io.File;
import java.io.InputStreamReader;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.apache.lucene.analysis.Analyzer;
import org.apache.lucene.analysis.SimpleAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.index.IndexReader;
import org.apache.lucene.queryParser.QueryParser;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.Searcher;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.MMapDirectory;
import org.apache.lucene.util.Version;

public class SearchSites {

	private SearchSites() {
	}

	private static long events = 0;

	/**
	 * Perform searches, outputting the search, appearance and click events.
	 * 
	 * Requires a sites index and a sessionup data file.
	 */
	public static void main(String[] args) throws Exception {
		IndexReader index = IndexReader.open(MMapDirectory.open(new File("index")), true);
		Searcher searcher = new IndexSearcher(index);
		Analyzer analyzer = new SimpleAnalyzer();
		QueryParser parser = new QueryParser(Version.LUCENE_30, "content", analyzer);

		BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
		String line = null;
		while ((line = reader.readLine()) != null) {
			String[] fields = line.split("\t");
			if ("AnonID".equals(fields[0]))
				return;
			String session = fields[0];
			String userId = fields[1];
			String query = fields[2];
			String occurredAt = fields[3];
			String rank = fields.length > 4 ? fields[4] : null;
			String clicked = fields.length > 5 ? fields[5] : null;

			TopDocs results = searcher.search(parser.parse(QueryParser.escape(query)), 100);
			print(session, userId, occurredAt, "search", query);
			if (clicked != null) {
				print(session, userId, occurredAt, "click", clicked, rank);
			}
			for (int i = 0; i < results.scoreDocs.length; i++) {
				ScoreDoc scoreDoc = results.scoreDocs[i];
				Document document = index.document(scoreDoc.doc);

				if (clicked != null) {
					print(session, userId, occurredAt, "appearance", document.get("url"), String.valueOf(i));
				}
			}
		}
		System.err.println("Total events: " + events);
		System.exit(0);
	}

	private static void print(String... args) throws Exception {
		boolean printTab = false;
		StringBuilder builder = new StringBuilder();
		for (String s : args) {
			if (printTab) builder.append('\t'); else printTab = true;
			builder.append(s);
		}
		System.out.println(builder.toString());
		events++;
	}

}
