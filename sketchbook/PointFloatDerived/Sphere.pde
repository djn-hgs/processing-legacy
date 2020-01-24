class Sphere extends Shape {
  PVector pos;
  float radius;
  
  Sphere(PVector p,float r) {
    radius = r;
    pos = p.get();
  }
  
  PVector getPoint() {
    float theta = random(255)/255 * PI;
    float phi = random(255)/255 * TWO_PI;
    float x = radius * sin(phi) * cos(theta);
    float y = radius * sin(phi) * sin(theta);
    float z = radius * cos(phi);
    PVector pnt = new PVector(x,y,z);
    pnt.add(pos);
    return pnt;
  }
}
