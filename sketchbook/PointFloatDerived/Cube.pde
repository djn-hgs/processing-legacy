class Cube extends Shape {
  PVector pos;
  float size;
  
  Cube(PVector p,float s) {
    size = s;
    pos = p.get();
  }
  
  PVector getPoint() {
    int face = int(random(6));
    float x = random(size)-size/2;
    float y = random(size)-size/2;
    PVector pnt = new PVector(0,0,0);
    switch(face) {
      case 0:
      pnt = new PVector(x,y,-size/2);
      break;
      case 1:
      pnt = new PVector(x,y,size/2);
      break;
      case 2:
      pnt = new PVector(x,-size/2,y);
      break;
      case 3:
      pnt = new PVector(x,size/2,y);
      break;
      case 4:
      pnt = new PVector(-size/2,x,y);
      break;
      case 5:
      pnt = new PVector(size/2,x,y);
    }  
    pnt.add(pos);
    return pnt;
  }
}
