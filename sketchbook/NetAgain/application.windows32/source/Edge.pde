class Edge {
  Vertex start;
  Vertex finish;
  PVector direction;
  
  Edge(Vertex s,Vertex f) {
    start = s;
    finish = f;
	direction = finish.pos.get();
	direction.sub(start.pos.get());
  }
  
  void display() {
    stroke(255,200);
    line(start.pos.x,start.pos.y,0,finish.pos.x,finish.pos.y,0);
  }  
}
