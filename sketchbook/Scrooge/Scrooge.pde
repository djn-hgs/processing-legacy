WordCloud myCloud;
WordStream myStream;

String[] tokens;
int counter = 0;
int readY;
int lastWord;

void setup() {
  frameRate(60);
  size(800, 600);
  readY=int(height/2);
  
  String[] lines = loadStrings("obama.txt");
  String allText = join(lines, " ").toLowerCase();
  tokens = splitTokens(allText, " ,.?!:;[]_-\"");

  // Create the font
  textFont(createFont("Georgia", 24));
  
  myCloud = new WordCloud();
  myStream = new WordStream();
}

void draw() {
  background(51);
  if (myStream.isOnScreen()) {
    if (counter < tokens.length) {
      String s = tokens[counter];
      counter++;
    
      if ( myCloud.hasWord(s) ) {
        myCloud.biggerWord(s);
      }
      else {
        myCloud.addWord( s, 18, new PVector( random(width), random(height)), new PVector(0 , 0),1);
      }

    myStream.addWord( s+" ", myCloud.wordSize(s), new PVector( width , readY ), new PVector( -4 , random(2)-1 ),1);
 
    }
  //camera(width/2,height/2,(height/2)/tan(PI/6),width/2,height/2,0,0,1,0);
  }
  
  myCloud.display();
  myStream.display();
  myCloud.update();
  myStream.update();
}
