class System {
  ArrayList< Body > bodyList;
  PVector pos;
  float G;
  
  System( PVector p , float G ) {
    pos = p.get();
    bodyList = new ArrayList< Body >();
  }

  System() {
    this( new PVector( 0 , 0 , 0 ) , 1 );
  }
  
  void add( Body b ) {
    bodyList.add( b );
  }
  
  void update() {
    for( Body b : bodyList ) {
      b.update();
    }
  }
  
  void draw() {
    for( Body b : bodyList ) {
      b.draw();
    }
  }
}

