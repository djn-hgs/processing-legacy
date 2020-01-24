class PointCloud {
  ArrayList<PVector> points;

  PointCloud(Sphere s, int pts) {
    points = new ArrayList<PVector>();
      for (int i=0; i<pts ; i++) {
      points.add(s.getPoint());
    }
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
  
}

