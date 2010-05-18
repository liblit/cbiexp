import java.awt.Color;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.Scanner;
import static java.lang.Math.log;

public class Main {

	public static void main(String[] args) {
		if (args.length != 1) {
			printUsage();
			System.exit(0);
		}

		String[][] info = readFile(args[0]);

		// Algorithm: generate a css file for each file
		// assume the following file format:
		// type,dir,fname,line_num,score

		// find all elements in the array that belong to
		// the same application and version

		writeToCSS(info);

	}

	// precondition: the array of strings is for one release
	// type,dir,fname,line_num,score,app,release
	public static void writeToCSS(String[][] info) {
		String html_fname;
		String src_fname;

		// create the necessary CSS files (overwrite the old ones)
		for (int i = 0; i < info.length; i++) {

			html_fname = info[i][2];
			src_fname = html_fname
					.substring(0, html_fname.lastIndexOf(".html"));

			try {  
			    File myFile = new File(info[i][1] + src_fname + "-" 
						+ info[i][0] + ".css");
				if (myFile.exists()) {
					myFile.delete();
				}

			} finally {
			}
		}

		// go through each element in the string and append it to an appropriate
		// file
		for (int i = 0; i < info.length; i++) {		 

			
			// parse filename from html file
			// build .css files in the format:
			// myfile-coverage.css
			// myfle-crashiness.css
			html_fname = info[i][2];
			File htmlFile =  new File(info[i][1] + info[i][2]);
			if (htmlFile.exists()) { 
			
				src_fname = html_fname
						.substring(0, html_fname.lastIndexOf(".html"));
	
				if (info[i][0].equals("coverage")) {
	
					//System.out.println("coverage");
	
					File myFile = new File(info[i][1] + src_fname + "-"
							+ info[i][0] + ".css");
					//System.out.println(myFile.getName());
					String linenum = info[i][3];
					String ln = "#line"
							+ linenum
							+ " { background-color: "
							+ getCSSColorFromScore(Integer.parseInt(info[i][4]),
									info[i][0]) + "}\n";
					try {
						FileWriter w = new FileWriter(myFile, true);
						w.write(ln);
						w.close();
					} catch (IOException e) {
						e.printStackTrace();
					}

				} else if (info[i][0].equals("crash-impact")) {
	
					File myFile = new File(info[i][1] + src_fname + "-"
							+ info[i][0] + ".css");
					//System.out.println(myFile.getName());
					String linenum = info[i][3];
					String ln = "#line"
							+ linenum
							+ " { background-color: "
							+ getCSSColorFromScore(Integer.parseInt(info[i][4]),
									info[i][0]) + "}\n";
					try {
						FileWriter w = new FileWriter(myFile, true);
						w.write(ln);
						w.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
	
				} else if (info[i][0].equals("crashiness")) {
	
					File myFile = new File(info[i][1] + src_fname + "-"
							+ info[i][0] + ".css");
					//System.out.println(myFile.getName());
					String linenum = info[i][3];
					String ln = "#line"
							+ linenum
							+ " { background-color: "
							+ getCSSColorFromScore(Integer.parseInt(info[i][4]),
									info[i][0]) + "}\n";
					try {
						FileWriter w = new FileWriter(myFile, true);
						w.write(ln);
						w.close();
					} catch (IOException e) {
						e.printStackTrace();
					}
				}
			} else {
				System.out.println("Ignoring missing file "+ info[i][1] + info[i][2]);
			}
		}
	}

    
    
    public static String getCSSColorFromScore(int score, String type) {

	//apply a logarithmic scale to the colors
	float normalizer = (float)log(102f);

		Color myColor;
		if (type.equals("coverage")) {
			// Mess with the numbers to get different effects
			// create a shade of blue: intensity 0-0.5
			//myColor = new Color(1.0f-score/100f, 1.0f-score/100f, 1.0f);//blue

			myColor = new Color((float)(log(101d-(double)score)/normalizer), 1.0f, (float)(log(101d-(double)score)/normalizer));//green
			/*
			myColor = new Color(1.0f, 1.0f-score/100f, 1.0f);//magenta
			myColor = new Color(1.0f, 1.0f, 1.0f-score/100f);//yellow
			myColor = new Color(1.0f-score/100f, 1.0f-score/100f, 1.0f-score/100f);//grey

			myColor = new Color(1.0f-score/100f, 1.0f, 1.0f);//cyan
			myColor = new Color(1.0f-score/100f, 1.0f, 1.0f-score/100f);//green
			myColor = new Color(1.0f-score/200f, 1.0f-score/200f, 1.0f);//blue-mix
			
*/
		} else if (type.equals("crash-impact")) {
			// create a shade of green
			myColor = new Color((float)(log(101d-(double)score)/normalizer), 1.0f, 1.0f);//cyan

		} else if (type.equals("crashiness")){
			// create a shade of yellow
			myColor = new Color(1.0f, 1.0f, (float)(log(101d-(double)score)/normalizer));//yellow
		} else {
			// Leave it white.
			myColor = new Color(1f, 1f, 1f);
		}

		String rgb = Integer.toHexString(myColor.getRGB());
		rgb = "#" + rgb.substring(2, rgb.length());
		return rgb;
	}



	public static void printUsage() {
		System.err.println("Usage: java Main <path to file>");
	}

	// takes the path of the file, reads it, and
	// returns a 2D array of strings with each element in the
	// file
	// Assumption: each line holds comma-separated values of the
	// data.
	public static String[][] readFile(String k) {
		File myFile;
		Scanner s = null;
		try {
			myFile = new File(k);
			s = new Scanner(myFile);
		}

		catch (FileNotFoundException e) {
			System.out.println("File does not exist or cannot be opened!");
			System.exit(0);
		}
		int numLines = 0;
		while (s.hasNextLine()) {
			numLines++;
			s.nextLine();
		}

		s.close();
		try {
			myFile = new File(k);
			s = new Scanner(myFile);
		} catch (Exception e) {
			System.out.println("Something wrong opening the file again");
		}

		String[][] results = new String[numLines][];
		for (int i = 0; i < numLines; i++) {
			results[i] = s.nextLine().split(",");
		}

		return results;

	}
}
