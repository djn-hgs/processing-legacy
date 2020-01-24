class Net {
	PVector pos;
	Face first;
	ArrayList<Join> joins;
	
	Net(PVector p) {
		pos = p.get();
		first = null;
		joins = new ArrayList<Join>();
	}
	
	Net(PVector p,Face f) {
		pos = p.get();
		first = f;
		joins = new ArrayList<Join>();
	}
	
	

	void base(Face f) {
		first = f;
	}
	
	void add(Face p,Edge e,Face c,float a) {
		joins.add(new Join(p,e,c,a));
	}
	
	void add(Face p,int n,Face c,float a) {
		joins.add(new Join(p,p.edge(n),c,a));
	}

	void display(float t) {
		pushMatrix();
		translate(pos.x,pos.y,pos.z);
		this.descendents(first,t);
		popMatrix();
		
	}
		
	void descendents(Face f,float t) {
	PVector dir;
	PVector pivot;
	PVector xdir = new PVector(1,0);
	f.display();
	for (Join j: joins) {
			if (j.parent==f) {
				pivot = j.edge.start();
				dir = j.edge.direction();
				pushMatrix();
				translate(pivot.x,pivot.y,pivot.z);
				rotate(angle(xdir,dir),0,0,1);
				rotateX(t*j.angle);
				descendents(j.child,t);
				popMatrix();
			}
		}
	}
	
	float angle(PVector v1,PVector v2) {
		float a = atan2(v2.y,v2.x) - atan2(v1.y,v1.x);
		if (a<0) a += TWO_PI;
		return a;
	}
}