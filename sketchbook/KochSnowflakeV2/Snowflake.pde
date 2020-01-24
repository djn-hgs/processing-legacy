class Snowflake {
  ArrayList<Triangle> triangles;
  PVector pt_A;
  PVector pt_B;
  PVector pt_C;
  
  Snowflake(PVector v_a, PVector v_b, PVector v_c) {
    triangles = new ArrayList<Triangle>();
    pt_A = v_a.get();
    pt_B = v_b.get();
    pt_C = v_c.get();
    
    triangles.add(new Triangle(pt_A , pt_B , pt_C));
  }
  
  void display() {
    pushMatrix();
    for (Triangle tri: triangles) {
      tri.display();
    }
    popMatrix();
  }
  
  void grow() {
    for (int i = triangles.size()-1; i >= 0 ; i--) {
      Triangle t = triangles.get(i);
      triangles.add(new Triangle(t.t1AB(),t.t2AB(),t.newC()));
    }
  }
}
