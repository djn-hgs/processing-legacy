
Histogram h1,h2,h3;
Distribution u;
Distribution n;
Demo myDemo;
color blue;
color red;
color green;
boolean uniform;

void setup() {
  size(800, 600);
  blue = color( 0 ,0 , 255 , 80 );
  red = color( 255 , 0 , 0 , 10 );
  green = color( 0 , 255 , 0 , 100 );
  h1 = new Histogram(100,600,600,400);
  h1.createBins(-3,3,20);
  h1.setColor(red);
  h2 = new Histogram(100,600,600,400);
  h2.createBins(-3,3,20);
  h2.setColor(blue);
  h3 = new Histogram(100,600,600,400);
  h3.createBins(-3,3,20);
  h3.setColor(green);
  u = new Uniform(-2,2);
  n = new Normal();
  uniform = true;
  myDemo = new Demo(u,h1,h2,h3,5);
  myDemo.setSampleColor(red);
  myDemo.setMeanColor(blue);
  myDemo.settStatColor(green);
  
  
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
  if(key == 'k' ) {
    myDemo.setRate(myDemo.rate+10);
  }
  if(key == 'm' ) {
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
}  

