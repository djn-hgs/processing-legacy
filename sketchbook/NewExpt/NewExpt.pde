import peasy.*;
Face myFace;
PeasyCam cam;
float t;

void setup() {
  frameRate(60);
  size(800,600,P3D);
  // noStroke();
  cam = new PeasyCam(this,200);
  
  myFace = new Face(
  }

void draw() {
  background(0);
  
  myFace.display();
}

  
