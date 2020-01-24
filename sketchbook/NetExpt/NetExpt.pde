import peasy.*;

PeasyCam cam;
float t;

void setup() {
  frameRate(60);
  size(800,600,P3D);
  // noStroke();
  cam = new PeasyCam(this,100);
  }

void draw() {
  background(0);
  
  t = mouseX/float(width);
  fill(200);
  rect(0.,0.,100.,100.);
  pushMatrix();
  translate(100,0);
  rotate(t*PI/2,0,1,0);
  fill(200);
  rect(0.,0.,100.,100.);
  pushMatrix();
  translate(100,0);
  rotate(t*PI/2,0,1,0);
  fill(200);
  rect(0.,0.,100.,100.);
  pushMatrix();
  rotate(-PI/2,0,0,1);
  rotate(t*PI/2,0,1,0);
  fill(200);
  rect(0.,0.,100.,100.);
  popMatrix();
  pushMatrix();
  translate(100,100);
  rotate(PI/2,0,0,1);
  rotate(t*PI/2,0,1,0);
  fill(200);
  rect(0.,0.,100.,100.);
  popMatrix();
  pushMatrix();
  translate(100,0);
  rotate(t*PI/2,0,1,0);
  fill(200);
  rect(0.,0.,100.,100.);
  popMatrix();
  popMatrix();
  popMatrix();
}

