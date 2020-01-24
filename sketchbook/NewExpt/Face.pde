class Face {
  ArrayList<Edge> edges;
    
  Face() {
    edges = new ArrayList<Edge>();
  }
  
  Face(ArrayList<PVector> pts) {
    PVector s = pts.get(0);
    Vertex vs = new Vertex(s);
    Vertex vv = vs.get();
    for (int i = 0 ; i<pts.size() ; i++ ) {
      PVector f = pts.get(i);
      Vertex vf = new Vertex(f);
      edges.add(new Edge(vs,vf));
      vs = vf;
    }
  }    
  
  void add(Edge e) {
    edges.add(e);
  }
  
   
  
  void display() {
    for (Edge e: edges) {
      e.display();
    }
    beginShape();
    Edge e = edges.get(0);
    vertex(e.start.pos.x,e.start.pos.y);
    for (int i = 0 ; i<edges.size() ; i++) {
       e = edges.get(i);
       vertex(e.finish.pos.x,e.finish.pos.y);
    }
    endShape();
  }
}
