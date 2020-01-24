Polygon p;
TriangleMesh m;
TriangleMesh c;


void setup() {
	frameRate(60);
	size(800,600);
	p = new Polygon(400,30,100,40);
	m = p.triangulate();
	c = new TriangleMesh( new PVector( 500 , 300 ) );
	c.perp(m);
	
}

void draw() {
	background(0);
	c.draw();
	c.update();
	if ( !c.isMoving() ) {
		c.reverse();
		}
		
}
