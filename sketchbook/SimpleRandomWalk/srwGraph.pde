class srwGraph {
  ArrayList<Float> myDists;
  PVector pos;
  float gWidth,gHeight,minD,maxD,cDist;
  boolean hasCurrent,restrict;
  int bCnt;
  
  srwGraph( float x , float y , float w , float h , int b , float m , boolean r ) {
    pos = new PVector( x , y );
    gWidth = w;
    gHeight = h;
    myDists = new ArrayList<Float>();
    hasCurrent = false;
    minD = 0;
    maxD = m;
    cDist = 0;
    bCnt = b;
    restrict = r;
  }
  
  void draw() {
    int i;
    int mCnt = 1;
    float bHeight;
    float dWidth = maxD - minD;
    float bWidth = dWidth/float(bCnt);
    int[] bins = new int[bCnt];
    for( i = 0 ; i<bCnt ; i++ ) {
      bins[i] = 0;
    }
    for( float d : myDists ) {
      i = int( ( d - minD )/bWidth);
      if (i==bCnt) i = bCnt -1;
      if (i<bCnt) {
        bins[i] = bins[i]+1;
        if (bins[i]>mCnt) mCnt = bins[i];
      }
    }
    
      
    pushMatrix();
    
    translate( pos.x , pos.y );
    pushMatrix();
    text(minD,0,gHeight+20);
    text(maxD,gWidth,gHeight+20);
    text(mCnt,-10,0);
    stroke(white);
    line(0,gHeight,0,0);
    line(0,gHeight,gWidth,gHeight);
    popMatrix();
    
    if (hasCurrent) {
      pushMatrix();
      translate((min(cDist,maxD)-minD)*gWidth/dWidth,0);
      stroke(white);
      line(0,0,0,gHeight);
      popMatrix();
    }
    
    for( i = 0 ; i<bCnt ; i++ ) {
      pushMatrix();
      translate(i*gWidth/float(bCnt),gHeight);
      bHeight = bins[i]/float(mCnt)*gHeight;
      beginShape();
      fill(150);
      vertex(0,0);
      vertex(0,-bHeight);
      vertex(gWidth/float(bCnt),-bHeight);
      vertex(gWidth/float(bCnt),0);
      endShape();
      popMatrix();
    }
    
    
    popMatrix();     
      
  }
  
  void current( float d ) {
    hasCurrent = true;
    cDist = d;
    //if (d<minD) minD = d;
    //if (d>maxD) maxD = d;
  }
  
  void add( float d ) {
    myDists.add(d);
    //if (d<minD) minD = d;
    if (!restrict) {
        if (d>maxD) maxD = d;
    }
  }
  
  
  void update() {
  }
}
