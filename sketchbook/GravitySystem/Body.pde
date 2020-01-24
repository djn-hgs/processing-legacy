class Body {
  PVector pos;
  PVector vel;
  float mass;
  float radius;
  
  Body( PVector p , PVector v , float m , float r ) {
    pos = p.get();
    vel = v.get();
    mass = m;
    radius = r;
  }
  
  void update() {
    pos.add( PVector.mult( vel , dt ) );
  }
  
  void draw() {
    pushMatrix();
    translate( pos.x , pos.y , pos.z );
    sphere( radius );
    popMatrix();
  }
}

