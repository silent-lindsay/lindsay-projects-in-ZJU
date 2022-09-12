/**
 * @author 25038
 * @version 1.0 
 * @date 2021/11/24
 * test for Java homework 4
 */
public class Main {
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
		
	public static void main(String[] args) {
		// TODO Auto-generated method stub
		Index index = new Index(IndexPath, ArgPostUrl, PG_NUM);
		index.createIndex();
		Geegle geegle = new Geegle(IndexPath);
		geegle.run();
	}
	
}
