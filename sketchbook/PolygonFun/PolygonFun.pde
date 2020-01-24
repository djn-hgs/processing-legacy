Polygon p;
TriangleMesh m,c,d;
int n;


void setup() {
	frameRate(60);
	size(800,600);
	n = 3;
	myPoly();
}

void draw() {
	background(0);
	c.draw();
	d.draw();
	c.update();
	d.update();
		
}

void keyPressed() {
	if (key=='l') {
		if (c.isMoving()) {
			c.reverse();
		}
		else {
			c.startMoving();
		}
	}
	if (key=='r') {
		if (d.isMoving()) {
			d.reverse();
		}
		else {
			d.startMoving();
		}
	}
	if (key=='u') {
		
		n = 2*n;		
		myPoly();
	}
	if (key=='d') {
		if (n>3) {
			n = n/2;
			myPoly();
		}
	}
		
}

void myPoly() {
	p = new Polygon(400,30,100,n);
	m = p.triangulate();
	c = new TriangleMesh( new PVector( 400 , 300 )  );
	d = new TriangleMesh( new PVector( 400 , 300 )  );
	c.leftSplit(m, 100);
	d.rightSplit(m, 100);
}