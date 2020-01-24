class Histogram {
  float xPos;
  float yPos;
  float histWidth;
  float histHeight;
  float histMin;
  float histMax;
  ArrayList<Bin> bins;
  color myColor;
  boolean visible;
  
  Histogram( float x , float y , float w , float h ) {
    xPos = x;
    yPos = y;
    histWidth = w;
    histHeight = h;
    bins = new ArrayList<Bin>();
    myColor = color(150);
    visible = true;
  }
  
  void setColor(color c) {
    myColor = c;
    for(Bin b: bins) {
      b.setColor(c);
    }
  }
  
  void createBins( float l , float r , int n ) {
    histMin = l;
    histMax = r;
    Bin b;
    float binWidth = (r - l) / n;
    float rectWidth = histWidth / n;
    for( int i = 0 ; i<n ; i++) {
      b =new Bin( xPos + i * rectWidth , yPos , rectWidth , histHeight , l + i*binWidth , l+(i+1) * binWidth );
      b.setColor(myColor);
      bins.add(b);
    }
    bins.get(n-1).setStrictRight();
    
  }
  
  void reZero() {
    for(Bin b: bins) {
      b.reZero();
    }
  }
  
  
  float min() {
    return histMin;
  }
  
  float max() {
    return histMax;
  }
  
  
  float getMax() {
    float m = 0;
    float n;
    float t = float( getTotal() );
    for(Bin b: bins) {
      n = b.get();
      if(n > m) {
      m = n;
      }
    }
    return m;
  }
  
  int getTotal() {
    int n = 0;
    for(Bin b: bins) {
      n = n + b.binCnt;
    }
    return n;
  }  
  
  float findScale() {
    return histHeight / getMax();
  }
  

  void setScale( float s ) {
    for(Bin b: bins) {
       b.setScale(s);
    } 
    
  }

  float getX(float v) {
    return xPos + histWidth * ( v - histMin ) / ( histMax - histMin );
  }
  
  void show() {
    visible = true;
  }
  
  void hide() {
    visible = false;
  }
  
  void flip() {
    if ( visible ) {
      visible = false;
    }
    else {
      visible = true;
    }
  }
  
  boolean isVisible() {
    return visible;
  }  
  
  void draw() {
    if ( visible ) {
      fill(myColor);
      for(Bin b: bins) {
        b.draw();
      }
    }
  }
  
  ArrayList<Float> calc( Sample s ) {
    return s.values;
  }
  
  void add( float v ) {
    for( Bin b: bins ) {
      if ( b.contains( v ) ) {
        b.increment();
      }
    }
  }
}
