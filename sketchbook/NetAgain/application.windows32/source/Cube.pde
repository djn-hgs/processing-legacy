class Cube extends Polyhedron {
	float s;
	
	Cube(float a,float x,float y,float z) {
	super(x,y,z);
	s = a;
	}
	
	void staircaseNet() {

    Face[] f = new Face[6];
	
	for (int i=0;i<6;i++) {
		f[i] = new Face(4,s);
	}
	
	myNet.base(f[0]);
	
	// Staircase
	
	myNet.add(f[0],3,f[1],PI/2);
	myNet.add(f[1],1,f[2],PI/2);
	myNet.add(f[2],3,f[3],PI/2);
	myNet.add(f[3],1,f[4],PI/2);
	myNet.add(f[4],3,f[5],PI/2);
	
	}
	
	
	void longNet() {
	Vertex v1,v2,v3,v4;
	Edge e1,e2,e3,e4;
	Face f1,f2,f3,f4,f5,f6;
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
