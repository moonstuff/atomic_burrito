package atomicburrito.indexsites;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileWriter;
import java.io.InputStreamReader;
import java.io.PrintWriter;
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

/** 
 * An attempt to perform concurrent searches
 *
 * It didn't really work out...
 */
public class ConcurrentSearchSites {

	private ConcurrentSearchSites() {
	}

	private static long events = 0;

	/**
	 * Perform searches
	 * 
	 * Requires a sites index and a sessionup data file.
	 */
	public static void main(String[] args) throws Exception {
		IndexReader index = IndexReader.open(MMapDirectory.open(new File("index")), true);
		Searcher searcher = new IndexSearcher(index);

		LinkedBlockingQueue<Runnable> queue = new LinkedBlockingQueue<Runnable>(10000);
		ThreadPoolExecutor pool = new ThreadPoolExecutor(1, 8, Long.MAX_VALUE, TimeUnit.NANOSECONDS, queue, new ThreadPoolExecutor.CallerRunsPolicy());
		
		try {
			BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
			String line = null;
			while ((line = reader.readLine()) != null) {
				SearchLine searchLineRunner = new SearchLine(searcher, index, line);
				pool.execute(searchLineRunner);
				events++;
			}
		} finally {
			while (pool.getQueue().size() > 0) {
				Thread.sleep(10);
			}
			pool.shutdown();
			System.err.println("Total events: " + events);
		}
		System.exit(0);
	}

	private static class SearchLine implements Runnable {

		private String line;

		private Searcher searcher;
		private Analyzer analyzer;
		private QueryParser parser;
		private IndexReader index;

		public SearchLine(Searcher searcher, IndexReader index, String line) {
			this.searcher = searcher;
			this.line = line;
			this.index = index;
		}

		private void searchLine(String line) throws Exception {
			this.analyzer = new SimpleAnalyzer();
			this.parser = new QueryParser(Version.LUCENE_30, "content", analyzer);

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

		private void print(String... args) throws Exception {
			boolean printTab = false;
			StringBuilder builder = new StringBuilder();
			for (String s : args) {
				if (printTab)
					builder.append('\t');
				else
					printTab = true;
				builder.append(s);
			}
			File f = new File(String.valueOf(Thread.currentThread().getId()));
			FileWriter fw = new FileWriter(f);
			PrintWriter pw = new PrintWriter(fw);
			pw.println(builder.toString());
			pw.close();
			fw.close();
		}

		@Override
		public void run() {
			try {
				searchLine(line);
			} catch (Exception e) {
				e.printStackTrace();
			}
		}

	}

}
