class PointCloud {
  ArrayList<PVector> points;

  PointCloud() {
    points = new ArrayList<PVector>();
  }
  
  void grow( Shape s, int pts) {
    for (int i=0; i<pts ; i++) {
      points.add( s.getPoint() );
    }
  }
  
  void add( PVector p ) {
    points.add( p.get() );
  }
  
  PointCloud cloneCloud() {
    PointCloud clone = new PointCloud();
    for (PVector p: points ) {
      clone.add( p.get() );
    }
    return clone;
  }
  
  
  void display() {
     stroke(255,255,255);
     for (PVector p: points) {
     point(p.x,p.y,p.z);
   }
  }
  
  
  PVector get(int i) {
    return points.get(i);
  }
  
  int size() {
    return points.size();
  }
 
  void update() {
  }
  
  void toSphere(float r, PVector pos) {
    for (PVector p: points) {
      p.normalize();
      p.mult(r);
      p.add(pos);
    }
  }  
}

