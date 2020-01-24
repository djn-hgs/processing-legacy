class Face {
  ArrayList<Edge> edges;
  
  Face() {
    edges = new ArrayList<Edge>();
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
