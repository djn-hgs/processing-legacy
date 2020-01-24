import peasy.*;
PeasyCam cam;
Polyhedron c , p ;
PVector i , j , k;
PMatrix3D m;

color black, white, blue;
float size;


void setup() {
  size(800,600,P3D); 
  cam = new PeasyCam(this,1000);
  strokeWeight(4);
  size = 100;
  black = color(50);
  blue = color (0 , 0 , 100 , 100 );
  white = color(255,50);
  
  i = new PVector( size , 0 , 0 );
  j = new PVector( 0 , size , 0 );
  k = new PVector( 0 , 0 , size );

  m = new PMatrix3D( 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 , 1 , 0 , 0 , 0 , 0 ,1 );
  
  c = new Cuboid();
  
  c.build( i , j , k );
  
  c.setColor(white);
  

}

void draw() {
  background( black );
  p = c.get();
  m.set( 2 , 3*float(mouseX)/height , 2  , 0 ,   5*float(mouseX)/height , 3*float(mouseY)/width, -1 , 0 ,   1.5*float(mouseX)/height , 0 , 4 , 0 ,    0 , 0 , 0  , 1 );
  p.applyMatrix( m );
  p.setColor( blue );
  c.draw();  
  p.draw();
}

