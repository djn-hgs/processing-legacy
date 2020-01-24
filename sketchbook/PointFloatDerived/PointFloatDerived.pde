import peasy.*;

PeasyCam cam;
PointCloud myCloud1;
PointCloud myCloud2;
Shape s1;
Shape s2;
Transition trans;
PVector pos1;
PVector pos2;
float rad1;
float rad2;
int points;
float t;
float step;
double x;
double y;

void setup() {
  colorMode(RGB, 100);
  frameRate(60);
  size(1024, 640,P3D);
  lights();
  sphereDetail(10);
  pos1 = new PVector(100,0,0);
  pos2 = new PVector(-400,0,0);
  rad1 = 400;
  rad2 = 400;
  points = 1000;
  s1 = new Cube( pos1 , rad1 );
  
  myCloud1 = new PointCloud();
  myCloud1.grow( s1 , points );
  myCloud2 = myCloud1.cloneCloud();
  myCloud2.toSphere( rad2 , pos2 );
  
  trans = new Transition(myCloud1,myCloud2);
  
  t=0;
  x=0;
  y=0;
  
  step = 0.01;
}

void draw() {
  background(0,0,0);
//  myCloud1.display();
//  myCloud2.display();
  //  stroke(255,255,255,255);
  noStroke();
  trans.snapshot(t);
  stroke(20,20,20,40);
  trans.display();
  t=t+(float(mouseX)/float(width)-0.5)/100;
  if (t>=1) {
    t=1;
  }
  
  if (t<=0) {
    t=0;
  }
  cam.setRotations( x, y, 0);
  x=x+(0.5-float(mouseY)/float(height))*PI/90;
  y=y+(0.5-float(mouseX)/float(width))*PI/90;  
}
