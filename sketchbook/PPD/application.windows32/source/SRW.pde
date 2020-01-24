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
