import org.jsoup.nodes.*;
import org.jsoup.select.Elements;
import org.wltea.analyzer.lucene.IKAnalyzer;
import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.TextField;
import org.apache.lucene.index.DirectoryReader;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.index.Term;
import org.apache.lucene.search.IndexSearcher;
import org.apache.lucene.search.Query;
import org.apache.lucene.search.ScoreDoc;
import org.apache.lucene.search.TermQuery;
import org.apache.lucene.search.TopDocs;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;
import org.jsoup.Jsoup;

import java.io.File;
import java.io.IOException;

/**
 * @author 25038
 * @version 1.0 
 * @date 2021/11/24
 * test for Java homework 4
 */
public class MainTest {
	/**
	 * @param args
	 */
	final static int PG_NUM = 50 ;
	final static String FIELD_COURSE = "course" ;
	final static String FIELD_COLLEGE = "college" ;
	final static String FIELD_PROFESSOR = "professor" ;
	final static String FIELD_INTRO = "introduction" ;
	final static String FIELD_URL = "url" ;
	final static String FIELD_COMMENT = "comment" ;
	final static String IndexPath = "./data_icourses/index" ;
	final static String BaseUrlString = "https://www.icourses.cn/cuoc" ;
	final static String ArgPostUrl = "https://www.icourses.cn/web//sword/portal/videoSearchPage?kw&catagoryId&listType=1" ;
	
	public static void createIndex(){
		IndexWriter iwr = null;
		try {
			iwr = new IndexWriter(FSDirectory.open(new File(IndexPath)),new IndexWriterConfig(Version.LUCENE_4_10_0, new IKAnalyzer()));			
			for ( int i = 1 ; i <= PG_NUM ; i++ ) {
				org.jsoup.nodes.Document coDocument = Jsoup.connect(ArgPostUrl).data("currentPage", String.valueOf(i)).post();
				Elements elements = coDocument.getElementsByClass("icourse-video-list-i") ;
				for (Element element : elements) 
					iwr.addDocument(getDocument(element));
				System.out.println("page "+i+" finished");
			}			
			iwr.close();
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}
	}
	
	public static Document getDocument(Element element){
		Document doc = new Document();
		String[] string = element.text().split(" ") ;
		doc.add(new TextField(FIELD_COURSE, string[0], Field.Store.YES));
		doc.add(new TextField(FIELD_COLLEGE,string[1], Field.Store.YES));
		doc.add(new TextField(FIELD_PROFESSOR,string[2],Field.Store.YES));
		StringBuilder intro = new StringBuilder() ;
		for ( int i = 3 ; i < string.length ; i++ ) intro.append(string[i]) ;
		String introString = intro.toString() ;
		doc.add(new TextField(FIELD_COMMENT, introString.substring(introString.lastIndexOf("。")+1), Field.Store.YES));
		introString = switchIntro(intro.toString()) ;
		doc.add(new TextField(FIELD_INTRO, introString.substring(0, introString.lastIndexOf("。")+1), Field.Store.YES));	
		doc.add(new TextField(FIELD_URL, element.getElementsByTag("a").attr("href"), Field.Store.YES));
		return doc;
	}
	
	public static String switchIntro(String string) {
		// TODO switch line every 60 words
		if ( string.length() > 60 ) 
			return string.substring(0,60)+"\n"+switchIntro(string.substring(60)) ;
		else 
			return string ;
	}

	public static void search(String fieldString, String targetString ){
		try {
			IndexSearcher searcher = new IndexSearcher(DirectoryReader.open(FSDirectory.open(new File(IndexPath))));
//			@SuppressWarnings("deprecation")
//			QueryParser parser = new QueryParser(Version.LUCENE_4_10_0,fieldString,  new IKAnalyzer() );
//			Query query = parser.parse(targetString);
			Query ttquery = new TermQuery(new Term(fieldString,targetString)) ;
			TopDocs hits = searcher.search(ttquery,10);
			System.out.println("查询到"+hits.totalHits+"记录");
			if ( hits.totalHits > 0 )
				hits = searcher.search(ttquery, hits.totalHits);
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
		} catch (IOException  e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		System.out.println("------------------------------------------------------------------------------------------");
	}
	
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		System.out.println("Search Engine based on icourses' data");
//		createIndex();	//		once create the index, no need to call createIndex again
		search(FIELD_COLLEGE, "浙江");
	}
	
	static void Print( Elements elements) {
		for (Element element : elements) {
			System.out.println(element.text()+ "\n" + element.getElementsByTag("a").attr("href")) ;
			System.out.println("---------------------------------");
		}
	}
}
