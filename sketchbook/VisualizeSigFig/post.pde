class post {
  PShape obj;
  PVector pos;
  float size;

  post( PVector p, float s ) {
    size = s;
    pos = p.get();
    obj = createShape( BOX, size );
    obj.setFill( 0 );
  }

  void display() {
    pushMatrix();
    translate( pos.x, pos.y, pos.z );
    shape( obj );
    popMatrix();
  }
}

