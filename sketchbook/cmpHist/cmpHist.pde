
Histogram h1;
normHist h2;
tHist  h3;
meanHist h4;
Distribution u;
Distribution n;
Demo myDemo;
color blue;
color red;
color green;
color purple;
boolean uniform;

void setup() {
  size(800, 600);
  blue = color( 0 ,0 , 255 , 80 );
  red = color( 255 , 0 , 0 , 10 );
  green = color( 0 , 255 , 0 , 100 );
  purple = color( 255 , 0 , 255 , 100 );
  h1 = new Histogram(100,600,600,400);
  h1.createBins(-3,3,20);
  h1.setColor(red);
  h2 = new normHist( 100 , 600 , 600 , 400 );
  h2.createBins(-3,3,20);
  h2.setColor(blue);
  h3 = new tHist( 100 , 600 , 600 , 400 );
  h3.createBins(-3,3,20);
  h3.setColor(green);
  h4 = new meanHist( 100 , 600 , 600 , 400 );
  h4.createBins(-3,3,20);
  h4.setColor(purple);
  u = new Uniform(-2,2);
  n = new Normal();
  uniform = true;
  myDemo = new Demo(u , h1 , 5);
  myDemo.add( h2 , 20 );
  myDemo.add( h3 , 20 );
  myDemo.add( h4 , 20 );

  
}

void draw() {
  background(140);
  myDemo.update();
  myDemo.draw();
}

void keyPressed() {
  if(key=='.') {
    myDemo.setSampleSize(myDemo.sampleSize+1);
  }
  if(key==',') {
    myDemo.setSampleSize(myDemo.sampleSize-1);
  }
  if(key == 'l' ) {
    myDemo.setRate(myDemo.rate+10);
  }
  if(key == ';' ) {
    myDemo.setRate(myDemo.rate-10);
  }
  if(key == ' ' ) {
    if(uniform) {
      uniform = false;
      myDemo.setDist( n );
    }
    else {
      uniform = true;
      myDemo.setDist( u );
    }
  }
  if ( key == 'd' ) {
    h1.flip();
  }
  if ( key == 'n' ) {
    h2.flip();
  }
  if ( key == 't' ) {
    h3.flip();
  }
  if ( key == 'm' ) {
    h4.flip();
  }
}  

