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
  physics.setfriction( .5f );

  Vec bPos;

  int gWidth = 20;
  int gHeight = 20;

   attr = new BAttraction( new Vec( mouseX , mouseY ) , 100 , 0.8 );
   physics.addBehavior( attr );

  for ( int i = 0; i <= gWidth; i++ ) {
    for ( int j = 0; j <= gHeight; j++ ) {

      bPos = new Vec( i * ( width / gWidth ), j * ( height / gHeight ) );

      VParticle myBlob = new VParticle( bPos, 5, 4 );
      myBlob.addBehavior( new BCollision() );
      blobs.add( myBlob );
      physics.addParticle( myBlob );

      VParticle myPin = new VParticle( bPos, 5, 4 );
      myPin.lock();
      pins.add( myPin );
      physics.addParticle( myPin );

      physics.addSpring( new VSpring( myPin, myBlob, 5f , 0.003f ) );
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
  stroke( 255 , 200 );
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
  
  stroke( 0, 40 );
  fill( 255, 40 );

  for ( VParticle b : pins ) {
    ellipse( b.x, b.y, b.getRadius() * 2, b.getRadius() * 2 );
  }
}