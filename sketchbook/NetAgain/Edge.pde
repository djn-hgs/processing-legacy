class Edge {
  Vertex start;
  Vertex finish;
  
	Edge(Vertex s,Vertex f) {
		start = s;
		finish = f;
	}
  
	void scale(float s,float t) {
		start.scale(s,t);
		finish.scale(s,t);
	}
  
	void display() {
		stroke(255,200);
		line(start().x,start().y,0,finish().x,finish().y,0);
	}
  
	PVector direction() {
		PVector d = finish.get();
		d.sub(start.get());
		return d;
	}
	
	PVector start() {
		return start.get();
	}
	
	PVector finish() {
		return finish.get();
	}
}
