package atomicburrito.indexsites;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;

import org.apache.lucene.analysis.standard.StandardAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;

/** 
 * Index sites from an mksites output file.
 *
 * We use Lucene because it's the simplest fast way to index data. It's pretty spiffy.
 *
 * The resulting index can be used by searchsites.
 */
public class IndexSites {

	private IndexSites() {
	}

	static final File INDEX = new File("index");
	
	static final int SITES_ARGNO = 0;

	public static void main(String[] args) throws Exception {
		if (args.length == 0) {
			System.err.println("Usage: java atomicburrito.indexsites.IndexSites <mksites_file>");
			System.exit(1);
		}
		IndexWriter writer = new IndexWriter(FSDirectory.open(INDEX), new StandardAnalyzer(Version.LUCENE_30), true,
				IndexWriter.MaxFieldLength.LIMITED);
		
		BufferedReader reader = new BufferedReader(new FileReader(new File(args[SITES_ARGNO])));
		String line = null;
		long i = 0;
		while ((line = reader.readLine()) != null) {
			String[] fields = line.split("\t");
			String url = fields[0];
			String content = fields[1];
			Document site = new Document();
			site.add(new Field("url", url, Field.Store.YES, Field.Index.NOT_ANALYZED));
			site.add(new Field("content", content, Field.Store.NO, Field.Index.ANALYZED));
			writer.addDocument(site);
			i = i + 1;
			if (i % 1000 == 0) {
				System.out.print(".");
				writer.optimize();
			}
		}
		writer.optimize();
		writer.close();
	}


}
