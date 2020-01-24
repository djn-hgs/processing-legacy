
Histogram h1,h2;
Distribution u;
Distribution n;
ParticleCloud p;
Demo myDemo;
color blue;
color red;
boolean uniform;

void setup() {
  size(800, 600);
  blue = color(0,0,255,100);
  red = color (255,0,0,20);
  h1 = new Histogram(100,600,600,150);
  h1.createBins(-3,3,50);
  h1.setColor(red);
  h2 = new Histogram(100,600,600,150);
  h2.createBins(-3,3,50);
  h2.setColor(blue);
  u = new Uniform(-2,2);
  n = new Normal();
  uniform = true;
  myDemo = new Demo(u,h1,h2,5);
  myDemo.setSampleColor(red);
  myDemo.setMeanColor(blue);
  
  
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

