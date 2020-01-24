class Cuboid extends Cube {
	float s,t,u;
	
	Cuboid(float a,float b,float c,float x,float y,float z) {
	super(1,x,y,z);
	s = a;
	t = b;
	u = c;
	f.get(0).scale(t,u);
	f.get(5).scale(t,u);
	f.get(1).scale(u,s);
	f.get(4).scale(u,s);
	f.get(2).scale(s,t);
	f.get(3).scale(s,t);
	}
	
	
}