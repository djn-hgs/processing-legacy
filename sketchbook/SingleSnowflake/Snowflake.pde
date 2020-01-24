class Snowflake {
  ArrayList<LineSeg> segs;
  PVector pos;
  PVector vel;
  float size;
  float rot;
  float rotspeed;
  PVector pt_A;
  PVector pt_B;
  PVector pt_C;
  PVector rad;
  int num;
  
  Snowflake(PVector p, PVector v, float sz, float rsp, int n) {
    segs = new ArrayList<LineSeg>();
    pos = p.get();
    vel = v.get();
    size = sz;
    rotspeed = rsp;
    num = n;
    rot = 0.0;
    rad = new PVector(0.0,size);
    pt_A = PVector.add(new PVector(0.0,0.0), rad);
    rad.rotate(-2.0*PI/3);
    pt_B = PVector.add(new PVector(0.0,0.0), rad);
    rad.rotate(-2.0*PI/3);
    pt_C = PVector.add(new PVector(0.0,0.0), rad);
       
    segs.add(new LineSeg(pt_A , pt_B ));
    segs.add(new LineSeg(pt_B , pt_C ));
    segs.add(new LineSeg(pt_C , pt_A ));
    
    for (int i=0; i<n ; i++) {
      grow();
    }

  }
  
  void display() {
    pushMatrix();
    translate(pos.x,pos.y);
    rotate(rot);
    for (LineSeg s: segs) {
      s.display();
    }
    popMatrix();
  }
  
  void update() {
    rot = rot + rotspeed;
    if (pos.y<height) {
    pos.add(vel);
    }
  }
  
  boolean issettled() {
    return (pos.y >= height); 
  }
  
  void grow() {
    for (int i = segs.size()-1; i >= 0 ; i--) {
      LineSeg s = segs.get(i);
      segs.add(new LineSeg(s.pt_A,s.t1AB()));
      segs.add(new LineSeg(s.t1AB(),s.tri()));
      segs.add(new LineSeg(s.tri(),s.t2AB()));
      segs.add(new LineSeg(s.t2AB(),s.pt_B));
      segs.remove(i);
    }
  }
}
