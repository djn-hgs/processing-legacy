class Tetrahedron extends Polyhedron {
	float s;
	
	Tetrahedron(float a,float x,float y,float z) {
	super(x,y,z);
	s = a;
	}
	
	void flowerNet() {

    Face[] f = new Face[4];
	
	for (int i=0;i<4;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Flower
	float a;
	a = PI-acos(1/3.);
	myNet.add(f[0],0,f[1],a);
	myNet.add(f[0],1,f[2],a);
	myNet.add(f[0],2,f[3],a);
	
	}
	
	void staircaseNet() {

    Face[] f = new Face[4];
	
	for (int i=0;i<4;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Staircase
	float a;
	a = PI-acos(1/3.);
	myNet.add(f[0],0,f[1],a);
	myNet.add(f[1],2,f[2],a);
	myNet.add(f[2],1,f[3],a);
	
	}
}