import peasy.*;

PeasyCam cam;
float t;

void setup() {
  frameRate(60);
  size(800,600,P3D);
  // noStroke();
  cam = new PeasyCam(this,200);
  }

void draw() {
  background(0);
  
  t = mouseX/float(width);
  
  translate(-50,-50);
  addFace();
  pushMatrix();
  translate(100,0);
  rotate(t*PI/2,0,1,0);
  addFace();
  pushMatrix();
  translate(100,0);
  rotate(t*PI/2,0,1,0);
  addFace();
  pushMatrix();
  rotate(-PI/2,0,0,1);
  rotate(t*PI/2,0,1,0);
  addFace();
  popMatrix();
  pushMatrix();
  translate(100,100);
  rotate(PI/2,0,0,1);
  rotate(t*PI/2,0,1,0);
  addFace();
  popMatrix();
  pushMatrix();
  translate(100,0);
  rotate(t*PI/2,0,1,0);
  addFace();
  popMatrix();
  popMatrix();
  popMatrix();
}

void addFace() {
  fill(200,200,200,130);
  rect(0.,0.,100.,100.);
}
  
