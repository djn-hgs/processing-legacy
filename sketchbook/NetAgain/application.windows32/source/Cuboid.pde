class Cuboid extends Polyhedron {
	Vertex v1,v2,v3,v4;
	Edge e1,e2,e3,e4;
	Face f1,f2,f3,f4,f5,f6;
	float s,t,u;
	
	Cuboid(float a,float b,float c,float x,float y,float z) {
	super(x,y,z);
	s = a;
	t = b;
	u = c;	
	}
	
	void staircaseNet() {
	
	Face[] f = new Face[6];
	
	f[0] = new Face(4,1);
	f[1] = new Face();
	f[2] = new Face();
	f[3] = new Face(f[2]);
	f[4] = new Face(f[1]);
	f[5] = new Face(f[0]);
	
	myNet.base(f[0]);
	
	// Staircase
	
	myNet.add(f[0],3,f[1],PI/2);
	myNet.add(f[1],1,f[2],PI/2);
	myNet.add(f[2],3,f[3],PI/2);
	myNet.add(f[3],1,f[4],PI/2);
	myNet.add(f[4],3,f[5],PI/2);
	
	}
	
	
	void longNet() {
	v1 = new Vertex(0,0);
	v2 = new Vertex(0,s);
	v3 = new Vertex(s,s);
	v4 = new Vertex(s,0);
	e1 = new Edge(v1,v2);
	e2 = new Edge(v2,v3);
	e3 = new Edge(v3,v4);
	e4 = new Edge(v4,v1);
    
	f1 = new Face();

	f1.add(e1);
	f1.add(e2);
	f1.add(e3);
	f1.add(e4);

	f2 = new Face(f1);
	f3 = new Face(f1);
	f4 = new Face(f1);
	f5 = new Face(f1);
	f6 = new Face(f1);
  
	myNet.first = f1;
	
	// Long
	myNet.add(f1,e2,f2,PI/2);
	myNet.add(f2,e2,f3,PI/2);
	myNet.add(f3,e2,f4,PI/2);
	myNet.add(f1,e1,f5,PI/2);
	myNet.add(f4,e3,f6,PI/2);

	}

}
	//v1 = new Vertex(0,0);
	//v2 = new Vertex(0,s);
	//v3 = new Vertex(s,s);
	//v4 = new Vertex(s,0);
	//e1 = new Edge(v1,v2);
	//e2 = new Edge(v2,v3);
	//e3 = new Edge(v3,v4);
	//e4 = new Edge(v4,v1);
    
	//PVector[] p = new PVector[4];
	//p[0] = new PVector(0,0);
	//p[1] = new PVector(s,0);
	//p[2] = new PVector(s,s);
	//p[3] = new PVector(0,s);
