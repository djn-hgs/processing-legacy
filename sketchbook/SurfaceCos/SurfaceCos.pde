import peasy.*;
PeasyCam cam;

SurfaceGrid s;
hGrid c;

void setup() {

  size( 800, 600, P3D );
  cam = new PeasyCam( this, 0, 0, 0, 500);
  cam.rotateX(PI/6);

  c = new CosGrid( 2 , 3, 10 , 10 , 100 );

  s = new SurfaceGrid( 200, 200, c );
//  s.build();
}

void draw() {
  background(240);
  s.display();
}

