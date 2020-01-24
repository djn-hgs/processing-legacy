class Grid {

  PShape g;
  
  PVector pos;
  PVector shape;
  int cWidth;
  int cHeight;
  int cDepth;
  
  Grid(PVector p,PVector s,int w,int h,int d) {
    pos = p.get(); 
    shape = s.get();
    cWidth = w;
    cHeight = h;
    cDepth = d;
    float wStep = shape.x/cWidth;
    float hStep = shape.y/cHeight;
    float dStep = shape.z/cDepth;
    g = createShape();
    g.beginShape(LINES);
    for (int j=0; j<=cHeight; j++) {
      for (int k=0; k<=cDepth; k++) {
      g.vertex(0.0,j*hStep,k*dStep);
      g.vertex(shape.x,j*hStep,k*dStep);
      }
    }
    for (int i=0; i<=cWidth; i++) {
      for (int k=0; k<=cDepth; k++) {
        g.vertex(i*wStep,0.0,k*dStep);
        g.vertex(i*wStep,shape.y,k*dStep);
      }
    }
    for (int i=0; i<=cWidth; i++) {
      for (int j=0; j<=cHeight; j++) {
        g.vertex(i*wStep,j*hStep,0.0);
        g.vertex(i*wStep,j*hStep,shape.z);
      }
    }
    g.endShape();
           
  }
  
  void update() {
  }
  
  void display() {
    translate(pos.x-shape.x/2,pos.y-shape.y/2,pos.z-shape.z/2);
    g.setFill(color(map(mouseX,0,width,0,255)));
    shape(g);
  }
}  
  
  
