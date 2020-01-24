class Histogram {
  float xPos;
  float yPos;
  float histWidth;
  float histHeight;
  float histMin;
  float histMax;
  ArrayList<Bin> bins;
  color myColor;
  
  Histogram( float x , float y , float w , float h ) {
    xPos = x;
    yPos = y;
    histWidth = w;
    histHeight = h;
    bins = new ArrayList<Bin>();
    myColor = color(150);
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
    bins.get(n-1).unsetStrictRight();
    
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
  
  
  void findScale() {
    float m = 1;
    float n;
    for(Bin b: bins) {
      n = b.get();
      if(n>m) {
        m = n;
      }
    }
    float s = histHeight / m;
    for(Bin b: bins) {
      b.setScale(s);
    }
  }
  
  void setScale( float s ) {
    for(Bin b: bins) {
       b.setScale(s);
    } 
    
  }

  float getX(float v) {
    return xPos + histWidth * ( v - histMin ) / ( histMax - histMin );
  }
  
    
  void draw() {
    findScale();
    fill(myColor);
    for(Bin b: bins) {
      b.draw();
    }
  }
  
  void add( float v ) {
    for(Bin b: bins) {
      if(b.contains(v)) {
        b.increment();
      }
    }
  }
}