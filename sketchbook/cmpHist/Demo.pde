class Demo {
  ArrayList< ParticleCloud > myCloud;
  Distribution dist;
  int sampleSize;
  int rate;
  int ctr;
  boolean moving;
  
  Demo( Distribution d, Histogram h , int n ) {
    dist = d;
    myCloud = new ArrayList< ParticleCloud >();
    myCloud.add( new ParticleCloud( h ) );
  
    sampleSize = n;
    moving = false;
    rate = 20;
    ctr = 0;
  }
  
  void add( Histogram h , float s ) {
    ParticleCloud p = new ParticleCloud( h );
    p.setSize( s );
    myCloud.add( p );
  }
  
  void setSampleSize(int n) {
    if(n>0) {
      sampleSize = n;
      for ( ParticleCloud p : myCloud ) {
        p.clear();
      }
    }
  }
  
  void setDist(Distribution d) {
      dist = d;
      for ( ParticleCloud p : myCloud ) {
        p.clear();
      }
  }
  
  void setRate(int n) {
    if ( n > 0 ) {
      rate = n;
    }
  }
  
  void update() {
    if( moving ) {
      moving = false;
      for ( ParticleCloud p : myCloud ) {
        p.update();
        if ( ! p.isDone() ) {
          moving = true;
        }
      }
    }
    
    if ( ctr==0 ) {
      Sample s = new Sample( sampleSize , dist );
      for ( ParticleCloud p : myCloud ) {
        p.calc(s);
      }
      moving = true;

    }
    
    ctr++;
    
    if( ctr>rate ) {
      ctr = 0;
    }
  }
  
  void draw() {
    float a , b;
    a = 100;
    for ( ParticleCloud p : myCloud ) {
        if ( p.destHist.isVisible() ) {
          b = p.destHist.findScale();
          if ( b < a ) {
            a = b;
          }
        }
    }
    
    
    
    for ( ParticleCloud p : myCloud ) {
        p.destHist.setScale( a );
        p.destHist.draw();
    }
    
    for ( ParticleCloud p : myCloud ) {
        p.draw();
    }
  }
}

  
  
    
