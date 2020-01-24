class Triangle {
  PVector pt_A;
  PVector pt_B;
  PVector pt_C;
  
  Triangle(PVector p_A,PVector p_B,PVector p_C) {
    pt_A=p_A.get();
    pt_B=p_B.get();
    pt_C=p_C.get();
  }
  
  void display() {
    stroke(255,255);
    triangle(pt_A.x,pt_A.y,pt_B.x,pt_B.y,pt_C.x,pt_C.y);
  }
  
  PVector mid_AB() {
    PVector v = PVector.add(pt_A,pt_B);
    v.mult(0.5);
    return (v);
  }
  
  PVector mid_BC() {
    PVector v = PVector.add(pt_B,pt_C);
    v.mult(0.5);
    return (v);
  }
   
  PVector mid_CA() {
    PVector v = PVector.add(pt_C,pt_A);
    v.mult(0.5);
    return (v);
  }
  
}
