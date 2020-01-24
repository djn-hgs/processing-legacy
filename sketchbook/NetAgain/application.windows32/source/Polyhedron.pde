class Polyhedron {
	Net myNet;
	PVector pos;
	
	Polyhedron(float x,float y,float z) {
		pos = new PVector(x,y,z);
		myNet = new Net(pos);
	}
		
	void display(float t) {
		myNet.display(t);

	}
}
