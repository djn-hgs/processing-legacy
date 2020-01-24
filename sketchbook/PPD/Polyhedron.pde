class Polyhedron {
  ArrayList<Face> faces;
  PVector pos;

  Polyhedron(PVector p) {
    pos = p.get();
    faces = new ArrayList<Face>();
  }

  void build( PVector i, PVector j , PVector k ) {
  }

  Polyhedron() {
    this( new PVector( 0 , 0 , 0 ) );
  }

  void face(Face f) {
    faces.add(f);
    
  }
  
  Polyhedron get() {
    Face g;
    Polyhedron p;
    p = new Polyhedron();
    for ( Face f : faces ) {
      g = new Face();
      g.clone( f );
      p.face( g );
    }
    return p;
  }
  
  
  void applyMatrix( PMatrix3D m ) {
    for ( Face f : faces ) {
      f.applyMatrix( m );
    }
  }
  
  void setColor(color c) {
    for( Face f : faces ) {
      f.setColor(c);
    }
  }
  
  void draw() {
    pushMatrix();
    translate( pos.x , pos.y , pos.z );
    for( Face f : faces ) {
      f.draw();
    }
    popMatrix();
  }
}

  
