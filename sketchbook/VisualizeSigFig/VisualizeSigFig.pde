import peasy.*;
//import processing.opengl.*;

PeasyCam cam;

ArrayList<numLine> lines;

void setup() {

  //size( 800, 600, OPENGL );
  size( 800, 600, P3D );

  cam = new PeasyCam( this, 100 );
  //cam.setMinimumDistance( 0 );
  //cam.setMaximumDistance( 200 );
  //cam.setYawRotationMode();

  lines = new ArrayList< numLine >();

  float left = -100;
  float right = 100;
  float step = 10;
  float multiplier = 10;
  float gap = 2;
  int places = 3;
  sphereDetail( 5 );

  for ( int i=0 ; i < places ; i++ ) {
    lines.add( new numLine( left, right, step, gap * (i + 1) ) );
    step = step / multiplier;
  }
}

void draw() {
  background(240);
  double d = cam.getDistance();
  float[] r = cam.getRotations();

  println(d+", "+r[0]+" , "+r[1]+" , "+r[2]);
  //translate( width / 2 , height / 2 );
  for ( numLine l : lines ) {
    l.display();
  }
}

