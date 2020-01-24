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
    noFill();
    triangle(pt_A.x,pt_A.y,pt_B.x,pt_B.y,pt_C.x,pt_C.y);
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
  PVector midBC()  {
   return param(pt_B, pt_C , 0.5);
  }
  PVector midCA()  {
   return param(pt_C, pt_A , 0.5);
  }

  PVector t1BC()  {
   return param(pt_B, pt_C , 1.0/3);
  }
 
  PVector t2BC()  {
   return param(pt_B, pt_C , 2.0/3);
  }
  PVector t1CA()  {
   return param(pt_C, pt_A , 1.0/3);
  }
 
  PVector t2CA()  {
   return param(pt_C, pt_A , 2.0/3);
  }
    
  PVector newA()  {
   return param(pt_A, midBC() , 4.0/3);
  }
  PVector newB()  {
   return param(pt_B, midCA() , 4.0/3);
  }
  PVector newC()  {
    return param(pt_C, midAB() , 4.0/3);
  }
 
}

