class Face {
  ArrayList<PVector> vertices;
  PVector pos;
  color myColor;
  
  Face(PVector p) {
    pos = p.get();
    vertices = new ArrayList<PVector>();
    myColor = color(150);
  }
  
  Face() {
    this(new PVector( 0 , 0 , 0 ) );
  }
  
  void clone( Face f ) {
    for( PVector v : f.vertices ) {
      vertices.add(v.get());
    }
    pos = f.pos.get();
    myColor = f.myColor;
  }
  
  void move( PVector t ) {
    pos.add(t);
  }
      
    void setColor( color c ) {
    myColor = c;
  }
  
  void vertex(PVector v) {
    vertices.add(v);
  }
  
  void draw() {
    PShape ps;
    
    ps = createShape();
    
    ps.beginShape();
    ps.stroke( myColor );
    
    for(PVector v : vertices) {
      ps.vertex( v.x , v.y , v.z );
    }
    
    ps.noFill();
    ps.endShape(CLOSE);
    //ps.setFill( myColor );
    ps.setStroke( true );
    ps.setStrokeWeight( 4 );
    
    pushMatrix();
    translate( pos.x , pos.y , pos.z );
    shape(ps);
    popMatrix();
    
  }
  
  void applyMatrix( PMatrix3D m ) {
    m.mult( pos , pos );
    for ( PVector v : vertices ) {
      m.mult( v , v );
    }
  }
  
}

