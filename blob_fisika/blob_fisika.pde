import org.processing.wiki.triangulate.*;
import punktiert.math.Vec;
import punktiert.physics.*;

// our world
VPhysics physics;

ArrayList triangles = new ArrayList();
ArrayList points = new ArrayList();

ArrayList< VParticle > pins = new ArrayList();
ArrayList< VParticle > blobs = new ArrayList(); 

BAttraction attr;

void setup() {
  size( 640, 480 );

  physics = new VPhysics( width, height );
  physics.setfriction( .1f );

  Vec bPos;

  int x_l = 40;
  int y_l = 40;
  int x_gap = 40;
  int y_gap = 40;
  int x_width = 10;
  int y_width = 10;

   attr = new BAttraction( new Vec( mouseX , mouseY ) , 100 , 0.1 );
   physics.addBehavior( attr );

  for ( int i = 0; i < x_width; i++ ) {
    for ( int j = 0; j < y_width; j++ ) {

      bPos = new Vec( x_l + i * x_gap, y_l + j * y_gap );

      VParticle myBlob = new VParticle( bPos, 5, 2 );
      blobs.add( myBlob );
      physics.addParticle( myBlob );

      VParticle myPin = new VParticle( bPos, 5, 2 );
      myPin.lock();
      pins.add( myPin );
      physics.addParticle( myPin );

      physics.addSpring( new VSpringRange( myPin, myBlob, 5, 10, 0.0003f ) );
    }
  }
}

void draw() {
  background( 0 );

  attr.setAttractor( new Vec( mouseX , mouseY ) );
  
  physics.update();
  
  
  points.clear();

  for ( VParticle b : blobs ) {
    points.add( new PVector( b.x, b.y ) );
  }       

  triangles = Triangulate.triangulate( points );

  // draw the mesh of triangles
  stroke( 255, 0, 0, 40 );
  fill( 255, 40 );

  beginShape(TRIANGLES);

  for (int i = 0; i < triangles.size(); i++) {
    Triangle t = (Triangle)triangles.get(i);
    vertex(t.p1.x, t.p1.y);
    vertex(t.p2.x, t.p2.y);
    vertex(t.p3.x, t.p3.y);
  }
  endShape();

  stroke( 0, 40 );
  fill( 255, 200 );

  for ( VParticle b : blobs ) {
    ellipse( b.x, b.y, b.getRadius() * 2, b.getRadius() * 2 );
  }
}