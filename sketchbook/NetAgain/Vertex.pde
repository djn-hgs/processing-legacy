class Vertex {
  PVector pos;
  PVector scale;
  
	Vertex(float x,float y) {
		pos = new PVector(x,y);
		scale = new PVector(1,1);
	}
  
    void scale(float s,float t) {
		scale.set(s,t);
	}

	PVector get() {
		PVector p = pos.get();
		p.x = p.x * scale.x;
		p.y = p.y * scale.y;
		return p;
	}
}