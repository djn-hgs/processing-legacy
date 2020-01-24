class Edge {
  Vertex from;
  Vertex to;
  
  Edge(Vertex f,Vertex t) {
    from = f;
    to = t;
  }
  
  void display() {
    line(from.pos.x,from.pos.y,to.pos.x,to.pos.y);
  }  
}
