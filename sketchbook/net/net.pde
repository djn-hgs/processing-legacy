import peasy.*;

PeasyCam cam;
Vertex v1;
Vertex v2;
Vertex v3;
Vertex v4;
Edge e1;
Edge e2;
Edge e3;
Edge e4;
Face myFace;

void setup() {
  frameRate(60);
  size(800,600,P3D);
  cam = new PeasyCam(this,100);
  //cam.setYawRotationMode();
  v1 = new Vertex(new PVector(-100,-100));
  v2 = new Vertex(new PVector(-100,100));
  v3 = new Vertex(new PVector(100,100));
  v4 = new Vertex(new PVector(100,-100));
  e1 = new Edge(v1,v2);
  e2 = new Edge(v2,v3);
  e3 = new Edge(v3,v4);
  e4 = new Edge(v4,v1);
  myFace = new Face();
  myFace.add(e1);
  myFace.add(e2);
  myFace.add(e3);
  myFace.add(e4);
}

void draw() {
  background(142);
  myFace.display();
}

