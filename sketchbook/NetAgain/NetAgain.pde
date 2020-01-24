import peasy.*;

ArrayList<Polyhedron> myNets;
float t;
PeasyCam cam;

void setup() {
	frameRate(60);
	size(800,600,P3D);
	cam = new PeasyCam(this,600);
	myNets = new ArrayList<Polyhedron>();

	Cube c = new Cube(50,100,-50,-50);
	c.longNet();
	myNets.add(c);
	
	Pyramid p = new Pyramid(50,-200,-50,-50);
	p.staircaseNet();
	myNets.add(p);
	
	Tetrahedron t = new Tetrahedron(50,0,-50,-50);
	t.flowerNet();
	myNets.add(t);
	
	Cuboid b = new Cuboid(50,10,20,300,-50,-50);
	b.staircaseNet();
	myNets.add(b);
	

}
 
void draw() {
  background(0);
  
  t = mouseX/float(width);
  
  for (Polyhedron c: myNets) {
	c.display(t);
  }
  
}
