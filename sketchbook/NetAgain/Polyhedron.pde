class Polyhedron {
	Net myNet;
	PVector pos;
	ArrayList<Face> f;

	
	Polyhedron(float x,float y,float z) {
		pos = new PVector(x,y,z);
		myNet = new Net(pos);
		f = new ArrayList<Face>();
	}

	void display(float t) {
		myNet.display(t);

	}
}