class Path {
  PVector start;
  PVector finish;
  
  float radius;
  
  Path(PVector s,PVector f) {
    start = s.get();
    finish = f.get();
  }
  
  PVector getPoint(float t) {
    PVector pos = finish.get();
    pos.sub(start);
    pos.mult(t);
    pos.add(start);
    return pos;
  }
  
  void snapshot(float t) {
    PVector pos = finish.get();
    pos.sub(start);
    pos.mult(t);
    pos.add(start);
    pushMatrix();
    translate(pos.x,pos.y,pos.z);
    sphere(2);
    popMatrix();
  }
  
  void display() {
    line(start.x,start.y,start.z,finish.x,finish.y,finish.z);
  }
}
