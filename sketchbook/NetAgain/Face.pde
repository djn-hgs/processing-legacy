class Face {
	PVector pos;
	ArrayList<Edge> edges;
  
	Face() {
		pos = new PVector(0,0,0);
		edges = new ArrayList<Edge>();
	}
	
	Face(PVector p,ArrayList<Edge> e) {
		pos = p.get();
		edges = e;
	}
  
	Face(Face f) {
		this(f.pos,f.edges);
	}
	
	Face(PVector[] p) {
		pos = new PVector(0,0,0);
		edges = new ArrayList<Edge>();
		int n = p.length;
		Vertex v,vs,vf;
		v = new Vertex(p[0].x,p[0].y);
		vs = v;
		for (int i=1;i<n;i++) {
		vf = new Vertex(p[i].x,p[i].y);
		edges.add(new Edge(vs,vf));
		vs = vf;
		}
		edges.add(new Edge(vs,v));
	}

	Face(int n,float s) {
		pos = new PVector(0,0,0);
		edges = new ArrayList<Edge>();
		Vertex v,vs,vf;
		PVector[] p = new PVector[n];
		float a = TWO_PI/float(n);
		p[0] = new PVector(s,0);
		for (int i=1;i<n;i++) {
			p[i] = new PVector(-s,0);
			p[i].rotate(-a*(i-1));
			p[i].add(p[i-1]);
		}
		v = new Vertex(p[0].x,p[0].y);
		vs = v;
		for (int i=1;i<n;i++) {
		vf = new Vertex(p[i].x,p[i].y);
		edges.add(new Edge(vs,vf));
		vs = vf;
		}
		edges.add(new Edge(vs,v));
	}
	
	void scale(float s,float t) {
		for(Edge e: edges) {
			e.scale(s,t);
		}
	}
 
	void add(Edge e) {
		edges.add(e);
	}
    
	void display() {
	//pushMatrix();
	translate(pos.x,pos.y);
    for (Edge e: edges) {
		e.display();
    }

    fill(100,80);

    beginShape();
    Edge e = edges.get(0);
    vertex(e.start().x,e.start().y);
    for (int i = 0 ; i<edges.size() ; i++) {
		e = edges.get(i);
		vertex(e.finish().x,e.finish().y);
    }
    e = edges.get(0);
    vertex(e.start().x,e.start().y);
    endShape();
	
	//popMatrix();
	}
	
	Edge edge(int n) {
		return edges.get(n);
	}
	
}
