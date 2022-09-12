import java.io.File;
import java.io.IOException;
import java.util.Scanner;

import org.apache.lucene.document.Document;
import org.apache.lucene.index.DirectoryReader;
import org.apache.lucene.queryparser.classic.ParseException;
import org.apache.lucene.queryparser.classic.QueryParser;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.wltea.analyzer.lucene.IKAnalyzer;

/**
 * @author 25038
 * 2021/11/27
 */
public class Geegle {
	private String IndexPath ;
	public Geegle(String path) {
		// TODO Auto-generated constructor stub
		IndexPath = path ;
	}
	static void Print( Elements elements) {
		for (Element element : elements) {
			System.out.println(element.text()+ "\n" + element.getElementsByTag("a").attr("href")) ;
			System.out.println("---------------------------------");
		}
	}

	private void search(String fieldString, String targetString) {
		// TODO Auto-generated method stub
		try {
			IndexSearcher searcher = new IndexSearcher(DirectoryReader.open(FSDirectory.open(new File(IndexPath))));
			@SuppressWarnings("deprecation")
			QueryParser parser = new QueryParser(Version.LUCENE_4_10_0,fieldString, new IKAnalyzer() );			
			Query query = parser.parse("\""+targetString+"\"");
			TopDocs hits = searcher.search(query,5);
			System.out.println("查询到"+hits.totalHits+"条记录");
			if ( hits.totalHits > 0 ) 
				hits = searcher.search(query, hits.totalHits) ;
			for(ScoreDoc doc:hits.scoreDocs){
				System.out.println("---------------------------------------------");
				Document d = searcher.doc(doc.doc);
				System.out.println("课程名称："+d.get(Main.FIELD_COURSE));
				System.out.println("开课院校："+d.get(Main.FIELD_COLLEGE));
				System.out.println("授课教师："+d.get(Main.FIELD_PROFESSOR));
//				System.out.println(d.get(Main.FIELD_INTRO));
//				System.out.println(d.get(Main.FIELD_COMMENT));
				System.out.println("课程链接："+d.get(Main.FIELD_URL));
			}
		} catch (IOException | ParseException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("------------------------------------------------------------------------------------------");
	}
	
	public void run() {
		System.out.println("Welcome to~ Search Engine powered by Geegle~ Search for any information of iourses here~");
		Scanner inScanner = new Scanner(System.in) ;
		while ( true ) 
		{
			System.out.println("Input 'exit' to quit the program, or input one of the fields followed to search:");
			System.out.println("options: '"+Main.FIELD_COURSE+"','"+Main.FIELD_PROFESSOR+"','"+Main.FIELD_COLLEGE+"'");
			String option = inScanner.next() ;
			if ( option.equals("exit") ) break ;
			System.out.print("Input search content: ");
			String tarString = inScanner.next() ;
			search(option, tarString);
		}
		inScanner.close();
		System.out.println("Bye!");
	}
	
}
