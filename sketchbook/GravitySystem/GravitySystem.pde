import peasy.*;

PeasyCam cam;
System s;
float dt;
PVector origin;

void setup() {
  frameRate(60);
  size(800, 600,P3D);
  sphereDetail( 10 );
  cam = new PeasyCam(this,100);
  
  dt = 0.01;
  
  s = new System();
  
  s.add( new Body( new PVector( 50 , 0 , 0 ) , new PVector( 0 , 10 , 0 ) , 1 , 4 ) );
  s.add( new Body( new PVector( 0 , 50 , 0 ) , new PVector( -10 , 0 , 0 ) , 1 , 4 ) );
  
  
  
}

void draw() {
  background( 200 );
  s.update();
  s.draw();
}
