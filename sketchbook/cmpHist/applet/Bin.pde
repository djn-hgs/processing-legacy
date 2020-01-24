class Bin {
  float binLeft;
  float binRight;
  int binCnt;
  boolean strictLeft;
  boolean strictRight;
  float xPos;
  float yPos;
  float binWidth;
  float binHeight;
  float binScale;
  color myColor;
  
  Bin( float x , float y , float w , float h , float l , float r ) {
    xPos = x;
    yPos = y;
    binWidth = w;
    binHeight = h;
    binLeft = l;
    binRight = r;
    binCnt = 0;
    binScale = 1;
    strictLeft = true;
    myColor = color(150);
    strictRight = false;
  }
  
  void setStrictLeft() {
    strictLeft = true;
  }
  
  void setStrictRight() {
    strictRight = true;
  }
  
  void setScale( float s ) {
    binScale = s;
  }
  
  void reZero() {
    binCnt = 0;
  }
  
  float midX() {
    return xPos+binWidth/2;
  }
  
  boolean contains( float v ) {
    boolean isIn = false;
    
    if( (v>binLeft) && (v<binRight) ) {
      isIn = true;
    }
  
    if( strictLeft && (v==binLeft) ) {
     isIn = true;
    }
   
    if( strictRight && (v==binRight) ) {
     isIn = true;
    }
   
    return isIn;
    
  }
  
  void increment() {
    binCnt++;
  }
  
  float get() {
    return binCnt/(binRight-binLeft);
  }
  
  void setColor(color c) {
    myColor = c;
  }
  
  void draw() {
    
    if ( (get() * binScale) > binHeight ) {
      println( get() + " and " + binScale + " and " + binHeight );
    }
    fill(myColor);
    rect(xPos,yPos,binWidth,-get()*binScale );
  }
}

