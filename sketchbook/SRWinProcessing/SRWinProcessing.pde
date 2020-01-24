import peasy.*;
PeasyCam cam;


color black, white;
srw myWalk;
srwGraph myGraph;



void setup() {
  size(1200,600,P3D);
  cam = new PeasyCam(this,400);
  strokeWeight(4);
    
  black = color(0);
  white = color(255);
  myGraph = new srwGraph( 200 , -200 , 200 , 100 , 20 , 200 , true );
  myWalk = new srw( 0 , 0 , 200, 100 , 3 , myGraph );
}

void draw() {
  background( black );
  myWalk.update();
  myWalk.draw();
  myGraph.draw();
  if (!myWalk.moving) {
    myWalk.restart();
  }  
}

