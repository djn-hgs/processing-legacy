class Demo {
  ParticleCloud p1;
  ParticleCloud p2;
  ParticleCloud p3;
  Histogram sampleHist;
  Histogram sampleMeanHist;
  Histogram tStatHist;
  Distribution dist;
  int sampleSize;
  int rate;
  int ctr;
  boolean moving;
  color sampleColor;
  color meanColor;
  color tStatColor;
  
  Demo( Distribution d, Histogram h1 , Histogram h2 , Histogram h3 , int n ) {
    dist = d;
    sampleHist = h1;
    sampleMeanHist = h2;
    tStatHist = h3;
  
    sampleSize = n;
    moving = false;
    rate = 20;
    ctr = 0;
    sampleColor = color(150);
    meanColor = color(150);
    tStatColor = color(150);
    p1 = new ParticleCloud( sampleHist );
    p2 = new ParticleCloud( sampleMeanHist );
    p3 = new ParticleCloud( tStatHist );
    
    p2.setSize(20);
    p3.setSize(20);
  }
  
  void setSampleColor(color c) {
    sampleColor = c;
    p1.setColor(c);
  }
  
  void setMeanColor(color c) {
     meanColor = c;
     p2.setColor(c);
  }
  
  void settStatColor(color c) {
     tStatColor = c;
     p3.setColor(c);
  }
  
  

  void setSampleSize(int n) {
    if(n>0) {
      sampleSize = n;
      p1.clear();
      p2.clear();
      p3.clear();
      sampleHist.reZero();
      sampleMeanHist.reZero();
      tStatHist.reZero();
    }
  }
  
  void setDist(Distribution d) {
      dist = d;
      p1.clear();
      p2.clear();
      p3.clear();
      sampleHist.reZero();
      sampleMeanHist.reZero();
      tStatHist.reZero();
  }
  
  void setRate(int n) {
    if(n>0) {
      rate = n;
    }
  }
  void update() {
    if(moving) {
      p1.update();
      p2.update();
      p3.update();
      if( (p1.isDone()) && (p2.isDone()) && (p3.isDone()) ) {
        moving = false;
      }
    }
    
    if(ctr==0) {
      Sample s = new Sample( sampleSize , dist );
      p1.add( s );
      //p2.add( s.normalize() );
      p2.add( s.mean() );
      p3.add( s.tStat() );
      moving = true;

    }
    ctr++;
    if(ctr>rate) {
      ctr = 0;
    }
  }
  
  void draw() {
    sampleHist.draw();
    sampleMeanHist.draw();
    //tStatHist.draw();
    p1.draw();
    p2.draw();
    //p3.draw();
  }
}

  
  
    