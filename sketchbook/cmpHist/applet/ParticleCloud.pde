class ParticleCloud {
  ArrayList<Ball> balls;
  float startHeight;
  Histogram destHist;
  boolean moving;
  float ballSize;
  color myColor;
  
  ParticleCloud( Histogram h1 ) {
    startHeight = 0;
    destHist = h1;
    balls = new ArrayList<Ball>();
    ballSize = 10;
    moving = false;
    myColor = destHist.myColor;
  }
  
  void setSize(float v) {
    ballSize = v;
    for(Ball b : balls) {
      b.setSize(v);
    }
  }
    
  void setColor(color c) {
    myColor = c;
    for(Ball b : balls) {
      b.setColor(c);
    }
  }
  
  void update() {
    boolean m = false;
    Ball b;
    for(int j = balls.size() -1; j>=0 ; j--) {
      b = balls.get(j);
      b.update();
      if( b.isDone() ) {
        destHist.add( b.getValue() );
        balls.remove(j);
      }
      else {
        m = true;
      }
      
      
    }
    moving = m;
    
  }
  
  boolean isDone() {
    if( moving ) {
      return false;
    }
    else {
      return true;
    }
  }
  
  void draw() {
    if ( destHist.isVisible() ) {
      
      for ( Ball b: balls ) {
        b.draw();
      }
    }
  }
  
  void clear() {
    moving = false;
    balls = new ArrayList<Ball>();
    destHist.reZero();
  }
  
  void add( float v ) {
    if ( (v > destHist.min()) && (v < destHist.max())) {
      Ball b = new Ball( destHist.getX(v) , startHeight , destHist.yPos , v );
      b.setSize( ballSize );
      b.setColor( myColor );
      balls.add( b );
      moving = true;
    }
  }
  
  void add( Sample s ) {
    for( float v : s.values ) {
      add( v );
    }
  }
  
  void calc( Sample s ) {
    ArrayList< Float > c = destHist.calc( s );
    for( float v : c ) {
      add( v );
    }
  }
}

