class Join {
	float angle;
	Edge edge;
	Face parent;
	Face child;
	
	Join(Face p,Edge e,Face c,float a) {
	parent = p;
	edge = e;
	child = c;
	angle = a;
	}
	
	
}