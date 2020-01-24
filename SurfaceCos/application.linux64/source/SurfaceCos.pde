import peasy.*;
PeasyCam cam;

PShape[][] myMesh;
Float[][] hData;
PShape s;
PShape m;
int cols;
int rows;
int hf;
int vf;
float gWidth;
float gHeight;
float hStep;
float vStep;
float amp;

void setup() {

  size( 800, 600, P3D );
  cam = new PeasyCam( this, 0,0,0,500);
  cam.rotateX(PI/6);
  gWidth = 200;
  cols = 50;
  hStep = gWidth/cols;
  hf = 2;
  gHeight = 200;
  rows = 50;
  vStep = gHeight/rows;
  vf =3;
  amp =100;
  

  hData = new Float[cols][rows]; 
  myMesh = new PShape[cols][rows];

  for (int i = 0 ; i<cols ; i++ ) {
    for ( int j=0 ; j<rows ; j++ ) {
      hData[i][j] = amp*cos(float(i*hf)/float(cols)*TWO_PI)*cos(float(j*vf)/float(rows)*TWO_PI);
    }
  }

  m = createShape(GROUP);

  for (int i = 1 ; i<cols ; i++ ) {
    for ( int j=1 ; j<rows ; j++ ) {
      s = createShape();
      s.beginShape();
      s.fill(255);
      s.stroke(0);
      s.vertex((i-1)*hStep, hData[i-1][j-1], (j-1)*vStep);
      s.vertex((i-1)*hStep, hData[i-1][j], j*vStep);
      s.vertex(i*hStep, hData[i][j], j*vStep);
      s.vertex(i*hStep, hData[i][j-1], (j-1)*vStep);
      s.endShape();
      m.addChild(s);
    }
  }
}

void draw() {
  background(240);
  shape(m, -(cols*hStep)/2, 0);
  
}
