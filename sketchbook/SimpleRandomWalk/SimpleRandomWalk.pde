import peasy.*;
PeasyCam cam;


color black, white;
srw myWalk;
srwGraph myGraph;



void setup() {
  // Parameters here are size of display (1200x600).
  size(800,600,P3D); //,P3D);
  // Parameter here is distance to viewpoint (400).
  cam = new PeasyCam(this,400);
  strokeWeight(4);
    
  black = color(0);
  white = color(255);
  // Parameters here are position of graph (200,-200), size of graph (200x100),
  // number of bins (20), scale on axis (200) and whether this is dynamically changed (true).
  myGraph = new srwGraph( 150 , -100 , 150 , 100 , 20 , 200 , false );
  // Parameters here are position of SRW (0,0), size to which it is rescaled (200),
  // number of dimensions (3) and graph to which results are sent (the one we just created).
  myWalk = new srw( 0 , 0 , 200, 100 , 4 , myGraph );
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

