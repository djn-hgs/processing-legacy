import peasy.*;
PeasyCam cam;


color black, white;
srw myWalk;
srwGraph myGraph;



void setup() {
  // Parameters here are size of display (1200x600).
  size(800,600,P3D); //,P3D);
  // Parameter here is distance to viewpoint (400).
  cam = new PeasyCam(this,400);
  strokeWeight(4);
    
  black = color(0);
  white = color(255);
  // Parameters here are position of graph (200,-200), size of graph (200x100),
  // number of bins (20), scale on axis (200) and whether this is dynamically changed (true).
  myGraph = new srwGraph( 150 , -100 , 150 , 100 , 20 , 200 , false );
  // Parameters here are position of SRW (0,0), size to which it is rescaled (200),
  // number of dimensions (3) and graph to which results are sent (the one we just created).
  myWalk = new srw( 0 , 0 , 200, 100 , 1 , myGraph );
}

void draw() {
  background( black );
  myWalk.update();
  myWalk.draw();
  myGraph.draw();
  if (!myWalk.moving) {
    myWalk.restart();
  }  
}

class srw {
  PVector base;
  nVect pos;
  int age,lifespan,dOf;
  boolean moving;
  float size,minx,maxx,miny,maxy,dist;
  srwGraph myGraph;
  ArrayList<nVect> pts;
  PShape pth;
  
  srw( float x , float y , float s, int l , int d , srwGraph g ) {
   base = new PVector( x , y );
   size = s;
   lifespan = l;
   age = 0;
   moving = true;
   minx = 0;
   maxx = 0;
   miny = 0;
   maxy = 0;
   dist = 0;
   dOf = d;
   pos = new nVect( dOf );
   myGraph = g;
   pts = new ArrayList<nVect>();
   
  }
  
  void update() {
    int i;
    if (moving) {
      pos.update();
      if (pos.x<minx) minx = pos.x;
      if (pos.x>maxx) maxx = pos.x;
      if (pos.y<miny) miny = pos.y;
      if (pos.y>maxy) maxy = pos.y;
      pts.add( new nVect(pos) );
      dist = pos.len();
      myGraph.current( dist );
      pth = createShape();
      pth.beginShape();
      pth.stroke(white);
      pth.noFill();
      i = 0;
      for( nVect p : pts ) {
        pth.stroke(float(i)/age*200+55);
        pth.vertex( p.x , p.y , p.z );
        i++;
      }
      pth.endShape();
      
      age++;
      if (age>lifespan) {
         moving = false;
         myGraph.add(dist);       
      }
    }
  }
  
  void restart() {
     pos = new nVect( dOf );
   
     age = 0;
     minx = 0;
     maxx = 0;
     miny = 0;
     maxy = 0;
     moving = true;
     pts = new ArrayList<nVect>();
    }
    
  void draw() {
    frameRate(50);
    float psize = max(maxx-minx,maxy-miny);
    pushMatrix();
    translate( base.x , base.y );
    scale(size/psize);
    shape(pth);
    popMatrix();
    
    pushMatrix();
    translate( base.x , base.y );
    scale(size/psize);
    translate( pos.x , pos.y , pos.z );
    pushMatrix();
    scale(psize/size);
    sphere(5);
    popMatrix();
    popMatrix();
    
  }   
}
class nVect {
  float x,y,z;
  float[] coords;
  int dim;
  
  nVect( int d ) {
    dim = d;
    coords = new float[dim];
    for( int i = 0 ; i<dim ; i++ ) {
      coords[i] = 0;
    }
      x = 0;
      y = 0;
      z = 0;
  }
  
  nVect( nVect v ) {
    dim = v.dim;
    coords = new float[dim];
    for( int i = 0 ; i<dim ; i++ ) {
      coords[i] = v.coords[i];
    }
    x = coords[0];
    if (dim<2) {
      y = 0;
    }
    else {
    y = coords[1];
    }
    if (dim<3) {
      z = 0;
    }
    else {
      z = coords[2];
    }
  }
    
  
  void update() {
    for( int i = 0 ; i<dim ; i++ ) {
      if (random(-1,1)<0) {
        coords[i]--;
      }
      else {
        coords[i]++;
      }
    }
    x = coords[0];
    if (dim<2) {
      y = 0;
    }
    else {
    y = coords[1];
    }
    if (dim<3) {
      z = 0;
    }
    else {
      z = coords[2];
    }
  }
  
  float len() {
    float l = 0;
    for( int i = 0 ; i<dim ; i++ ) {
      l = l + coords[i]*coords[i];
    }
    return l;
  }
}  
  
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

