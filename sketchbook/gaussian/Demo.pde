class Demo {
  ParticleCloud p1;
  ParticleCloud p2;
  Histogram sampleHist;
  Histogram sampleMeanHist;
  Distribution dist;
  int sampleSize;
  int rate;
  int ctr;
  boolean moving;
  color sampleColor;
  color meanColor;
  
  Demo( Distribution d, Histogram h1 , Histogram h2 , int n ) {
    dist = d;
    sampleHist = h1;
    sampleMeanHist = h2;
    sampleSize = n;
    moving = false;
    rate = 20;
    ctr = 0;
    sampleColor = color(150);
    meanColor = color(150);
    p1 = new ParticleCloud(sampleHist);
    p2 = new ParticleCloud(sampleMeanHist);
    p2.setSize(20);
  }
  
  void setSampleColor(color c) {
    sampleColor = c;
    p1.setColor(c);
  }
  
  void setMeanColor(color c) {
     meanColor = c;
     p2.setColor(c);
  }
  
  void setSampleSize(int n) {
    if(n>0) {
      sampleSize = n;
      p1.clear();
      p2.clear();
      sampleHist.reZero();
      sampleMeanHist.reZero();
    }
  }
  
  void setDist(Distribution d) {
      dist = d;
      p1.clear();
      p2.clear();
      sampleHist.reZero();
      sampleMeanHist.reZero();
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
      if((p1.isDone())&&(p2.isDone())) {
        moving = false;
      }
    }
    
    if(ctr==0) {
      float sum = 0;
      float v;
      for(int i = 0 ; i<sampleSize ; i++) {
        v = dist.get();
        p1.add(v);
        sum = sum + v;
      }
      p2.add(sum/sampleSize);
      moving = true;

    }
    ctr++;
    if(ctr>rate) {
      ctr = 0;
    }
  }
  
  void draw() {
    p1.draw();
    p2.draw();
  }
}

  
  
    
