import java.io.File;

import org.apache.lucene.document.Document;
import org.apache.lucene.document.Field;
import org.apache.lucene.document.TextField;
import org.apache.lucene.index.IndexWriter;
import org.apache.lucene.index.IndexWriterConfig;
import org.apache.lucene.store.FSDirectory;
import org.apache.lucene.util.Version;
import org.jsoup.Jsoup;
import org.jsoup.nodes.Element;
import org.jsoup.select.Elements;
import org.wltea.analyzer.lucene.IKAnalyzer;

/**
 * @author 25038
 * 2021/11/27
 */
public class Index {
	/**
	 * parameters 
	 */
	private String IndexPath ;
	private String UrlString ;
	private int PageNumber ;
	
	public Index(String path, String url , int number ) {
		// TODO Auto-generated constructor stub
		IndexPath = path ;
		UrlString = url ;
		PageNumber = number ;
	}
	
	public void createIndex(){
		if ( new File(IndexPath).exists() ) return ;
		IndexWriter iwr = null;
		try {
			iwr = new IndexWriter(FSDirectory.open(new File(IndexPath)),new IndexWriterConfig(Version.LUCENE_4_10_0, new IKAnalyzer()));			
			for ( int i = 1 ; i <= PageNumber ; i++ ) {
				org.jsoup.nodes.Document coDocument = Jsoup.connect(UrlString).data("currentPage", String.valueOf(i)).post();
				Elements elements = coDocument.getElementsByClass("icourse-video-list-i") ;
				for (Element element : elements) 
					iwr.addDocument(getDocument(element));
			}			
			iwr.close();
		} catch (Exception e) {
			// TODO: handle exception
			e.printStackTrace();
		}
		System.out.println("Index created!");
	}
	
	private Document getDocument(Element element){
		Document doc = new Document();
		String[] string = element.text().split(" ") ;
		doc.add(new TextField(Main.FIELD_COURSE, string[0], Field.Store.YES));
		doc.add(new TextField(Main.FIELD_COLLEGE,string[1], Field.Store.YES));
		doc.add(new TextField(Main.FIELD_PROFESSOR,string[2],Field.Store.YES));
		StringBuilder intro = new StringBuilder() ;
		for ( int i = 3 ; i < string.length ; i++ ) intro.append(string[i]) ;
		String introString = intro.toString() ;
		doc.add(new TextField(Main.FIELD_COMMENT, introString.substring(introString.lastIndexOf("¡£")+1), Field.Store.YES));
		introString = switchIntro(intro.toString()) ;
		doc.add(new TextField(Main.FIELD_INTRO, introString.substring(0, introString.lastIndexOf("¡£")+1), Field.Store.YES));	
		doc.add(new TextField(Main.FIELD_URL, element.getElementsByTag("a").attr("href"), Field.Store.YES));
		return doc;
	}
	
	private String switchIntro(String string) {
		// TODO switch line every 60 words
		if ( string.length() > 60 ) 
			return string.substring(0,60)+"\n"+switchIntro(string.substring(60)) ;
		else 
			return string ;
	}
	
}
