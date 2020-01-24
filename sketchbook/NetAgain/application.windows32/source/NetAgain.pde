import peasy.*;

ArrayList<Polyhedron> myNets;
float t;
PeasyCam cam;

void setup() {
	frameRate(60);
	size(800,600,P3D);
	cam = new PeasyCam(this,400);
	myNets = new ArrayList<Polyhedron>();

	Cube c = new Cube(50,200,-50,-50);
	c.staircaseNet();
	myNets.add(c);
	
	Pyramid p = new Pyramid(100,-200,-50,-50);
	p.staircaseNet();
	myNets.add(p);
	
	Tetrahedron t = new Tetrahedron(100,0,-50,-50);
	t.staircaseNet();
	myNets.add(t);
	

}
 
void draw() {
  background(0);
  
  t = mouseX/float(width);
  
  for (Polyhedron c: myNets) {
	c.display(t);
  }
  
}
