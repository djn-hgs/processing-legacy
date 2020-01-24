class Pyramid extends Polyhedron {
	float s;
	
	Pyramid(float a,float x,float y,float z) {
	super(x,y,z);
	s = a;
	}
	
	void flowerNet() {

    Face[] f = new Face[5];
	
	f[0]=new Face(4,s);
	
	for (int i=1;i<5;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Flower
	float a;
	a = PI-acos(1/sqrt(3));
	myNet.add(f[0],0,f[1],a);
	myNet.add(f[0],1,f[2],a);
	myNet.add(f[0],2,f[3],a);
	myNet.add(f[0],3,f[4],a);
	
	}
	
	void staircaseNet() {

    Face[] f = new Face[5];
	
	f[0]=new Face(4,s);
	
	for (int i=1;i<5;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Staircase
	float a;
	a = acos(1/sqrt(3));
	myNet.add(f[0],0,f[1],PI-a);
	myNet.add(f[1],2,f[2],PI-2*a);
	myNet.add(f[2],1,f[3],PI-2*a);
	myNet.add(f[3],1,f[4],PI-2*a);
	
	}
}
