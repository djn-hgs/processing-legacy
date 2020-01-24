import peasy.*;

PeasyCam cam;

PShape l;


void setup() {
  size(800, 600, P3D);
  cam = new PeasyCam(this,100);
  cam.setMinimumDistance(-500);
  cam.setMaximumDistance(500);
  //cam.setYawRotationMode();

  float dWidth = 10;
  int dPlaces = 3;
  float dGap = 50;
  float dSpace = 50;
  float mid = 0;
  float size = 1;
  float step = size;
  int nLines;
  
  
  l = createShape();
  l.beginShape(LINES);
  l.stroke(255);
  for(int i=0 ; i<dPlaces ; i++) {
    println(i);
    nLines=int(dWidth/step);
    for(int j=0 ; j <= nLines ; j++ ) {
      println(j);
      l.vertex((mid+(j-nLines/2)*dGap)*step,20,-i*dSpace);
      l.vertex((mid+(j-nLines/2)*dGap)*step,20,-dPlaces*dSpace);
    }
    step = step/10;
  }
  l.endShape();
  
  
}

void draw() {
  background(0);
  shape(l);
}

