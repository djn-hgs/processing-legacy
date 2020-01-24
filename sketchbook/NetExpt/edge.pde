class Edge {
  Vertex start;
  Vertex finish;
  
  Edge(Vertex s,Vertex f) {
    start = s;
    finish = f;
  }
  
  void display() {
    line(start.pos.x,start.pos.y,0,finish.pos.x,finish.pos.y,0);
  }  
}
