import java.io.File;
import java.io.FileWriter;
import java.awt.Color;
import java.io.IOException;
//import Main;
//import static Main.getCSSColorFromScore;
import static java.lang.Math.log;

public class CreateMainCSS {

    public static void main(String args[]) throws IOException
    {
	//open maintable.css for writing
	FileWriter maincss = new FileWriter(new File("maintable.css"), false);
					
	String thisColor;
	
	//get, write coverage colors
	for (int score=0; score<101; score++) {
	    thisColor = getCSSColorFromScore(score, "coverage");
	    maincss.write("#coverage" + score + " { background-color: " + thisColor + " }\n");
	}
					     
	//get, write crash-impact colors
	for (int score=0; score<101; score++) {
	    thisColor = getCSSColorFromScore(score, "crash-impact");
	    maincss.write("#crash-impact" + score + " { background-color: " + thisColor + " }\n");
	}

	//get, write crashiness colors
	for (int score=0; score<101; score++) {
	    thisColor = getCSSColorFromScore(score, "crashiness");
	    maincss.write("#crashiness" + score + " { background-color: " + thisColor + " }\n");
	}

	//close css file
	maincss.close();

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

}
