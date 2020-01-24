class ParticleCloud {
  ArrayList<Ball> balls;
  float startHeight;
  Histogram destHist;
  boolean moving;
  float ballSize;
  color myColor;
  
  ParticleCloud(Histogram h) {
    startHeight = 0;
    destHist = h;
    balls = new ArrayList<Ball>();
    ballSize = 10;
    moving = false;
    myColor = color(150);
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
      if(b.isDone()) {
        destHist.add(b.getValue());
        balls.remove(j);
      }
      else {
        m = true;
      }
      
      
    }
    moving = m;
    
  }
  
  boolean isDone() {
    if(moving) {
      return false;
    }
    else {
      return true;
    }
  }
  
  void draw() {
    destHist.draw();
    for(Ball b: balls) {
      b.draw();
    }
    
  }
  
  void clear() {
    moving = false;
    balls = new ArrayList<Ball>();
  }
  
  void add(float v) {
    if ( (v>destHist.min()) && (v<destHist.max())) {
    Ball b = new Ball(destHist.getX(v),startHeight,destHist.yPos,v);
    b.setSize(ballSize);
    b.setColor(myColor);
    balls.add(b);
    moving = true;
    }
  }
  
}

