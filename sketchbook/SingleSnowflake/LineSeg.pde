class LineSeg  {
  PVector pt_A;
  PVector pt_B;
  
  LineSeg( PVector p_A, PVector p_B ) {
    pt_A=p_A.get();
    pt_B=p_B.get();
  }
  
  void display() {
    stroke(255,255);
    line(pt_A.x,pt_A.y,pt_B.x,pt_B.y);
  }
  
    PVector param(PVector A, PVector B, float t) {
    PVector v = PVector.sub(B,A);
    v.mult(t);
    v.add(A);
    return(v);
  }
  
  PVector t1AB()  {
   return param(pt_A, pt_B , 1.0/3);
  }
 
  PVector t2AB()  {
   return param(pt_A, pt_B , 2.0/3);
  }

  PVector midAB()  {
   return param(pt_A, pt_B , 0.5);
  }
  PVector tri() {
    PVector v = PVector.sub(t2AB(),t1AB());
    v.rotate(PI/3);
    v.add(t1AB());
    return(v);
  }
}
