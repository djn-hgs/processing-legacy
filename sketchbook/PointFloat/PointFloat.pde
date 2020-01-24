import peasy.*;

PeasyCam cam;
PointCloud myCloud1;
PointCloud myCloud2;
Sphere s1;
Sphere s2;
Transition trans;
PVector pos1;
PVector pos2;
float rad1;
float rad2;
int points;
float t;
float step;

void setup() {
  colorMode(RGB, 100);
  frameRate(60);
  size(800, 600,P3D);
  lights();
  sphereDetail(20);
  cam = new PeasyCam(this,100);
  cam.setYawRotationMode();
  pos1 = new PVector(300,0,0);
  pos2 = new PVector(-200,0,0);
  rad1 = 400;
  rad2 = 200;
  points = 500;
  s1 = new Sphere(pos1,rad1);
  s2 = new Sphere(pos2,rad2);
  
  myCloud1 = new PointCloud(s1,points);
  myCloud2 = new PointCloud(s2,points);
  
  trans = new Transition(myCloud1,myCloud2);
  
  t=0;
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
  t=t+step;
  if (t>=1) {
    step = -step;
  }
  
  if (t<=0) {
    step = -step;
  }
  
}
