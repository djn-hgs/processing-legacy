class Gasket {
  ArrayList<Triangle> triangles;  // All the triangles in the gasket
  PVector pt_A;                 // Location of the gasket
  PVector pt_B;
  PVector pt_C;
  
  Gasket(PVector v_a, PVector v_b, PVector v_c) {
    triangles = new ArrayList<Triangle>();  // Initialize our triangle list
    pt_A = v_a.get();                       // Store the origin
    pt_B = v_b.get();
    pt_C = v_c.get();
    
    triangles.add(new Triangle(pt_A , pt_B , pt_C));
  }

  void display() {
    for (Triangle tri: triangles) {
      tri.display();
    }
  }
  
  void grow() {
    for (int i = triangles.size()-1; i >= 0 ; i--) {
      Triangle t = triangles.get(i);
      triangles.add(new Triangle( t.mid_AB(), t.pt_B , t.mid_BC() ));
      triangles.add(new Triangle( t.mid_BC(), t.pt_C , t.mid_CA() ));
      triangles.add(new Triangle( t.mid_CA(), t.pt_A , t.mid_AB() ));
      
    }
  }
}
