// http://wiki.processing.org/w/Triangulation
import org.processing.wiki.triangulate.*;
 
ArrayList triangles = new ArrayList();
ArrayList points = new ArrayList();

PVector lastPoint;
 
void setup()
{
  size( 640 , 480 );
  smooth();
  
  lastPoint = new PVector( width / 2 , height / 2 );
  
  points.add(new PVector( random( width ) , random( height ) ));
  points.add(new PVector( random( width ) , random( height ) ));
  points.add(new PVector( random( width ) , random( height ) ));
  points.add( lastPoint );
}
 
void draw()
{
  background(200);
 
  lastPoint.x = mouseX;
  lastPoint.y = mouseY;
  
  triangles = Triangulate.triangulate( points );

  // draw the mesh of triangles
  stroke(0, 40);
  fill(255, 40);
  
  beginShape(TRIANGLES);
 
  for (int i = 0; i < triangles.size(); i++) {
    Triangle t = (Triangle)triangles.get(i);
    vertex(t.p1.x, t.p1.y);
    vertex(t.p2.x, t.p2.y);
    vertex(t.p3.x, t.p3.y);
  }
  endShape();
}

void mousePressed()
{
  lastPoint = new PVector( mouseX , mouseY );
  points.add( lastPoint );
}