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
class Polygon {
	PVector pos;
	float radius;
	int sides;
	
	Polygon( float x , float y , float r , int n ) {
		pos = new PVector( x , y );
		radius = r;
		sides = n;
	}
	
	TriangleMesh triangulate() {
		Triangle nt;
		float na;
		float a = TWO_PI/float( sides );
		Triangle t = new Triangle( radius , a );
		m = new TriangleMesh( pos );
		for( int i=0 ; i<sides ; i++ ) {
			nt = new Triangle(t);
			nt.triRotate( i*a );
			m.add( nt );
		}
	return m;
	}
}
class Triangle {
	PVector p1 , p2 , p3;
	PVector dest , pos;
	float fromAngle, myAngle , finA;
	boolean isMoving;
	boolean forward; 
	int t , steps;
	
	
	Triangle( PVector q1 , PVector q2 , PVector q3 , float a) {
		p1 = q1.get();
		p2 = q2.get();
		p3 = q3.get();
		pos = new PVector(0,0);
		dest = new PVector(0,0);
		myAngle = a;
		fromAngle = a;
		finA = 0;
		isMoving = false;
		forward = true;
				
	}
	
	Triangle( float r , float a ) {
		this( new PVector( r , 0 ) , new PVector( 0 , 0 ) , new PVector( r*cos(a) , r*sin(a) ) , 0 );
	}
	
	Triangle( Triangle t ) {
		this( t.p1 , t.p2 , t.p3 , t.fromAngle );
	}
	
	void triRotate( float a ) {
		fromAngle = a;
		myAngle = a;
	}
	
	void setDest( PVector d , float a , int s) {
		forward = true;
		dest = d.get();
		finA = a;
		t = 0;
		steps = s;
	}
	
	PVector p3dest() {
		PVector p;
		p = p3.get();
		p.rotate(finA);
		p.add(dest);
		return p;
	}
	
	PVector p1dest() {
		PVector p;
		p = p1.get();
		p.rotate(finA);
		p.add(dest);
		return p;
	}
	
	PVector p2dest() {
		PVector p;
		p = p2.get();
		p.rotate(finA);
		p.add(dest);
		return p;
	}
	
	PVector mid31() {
		PVector m;
		m = p3.get();
		m.sub(p1);
		m.mult(0.5);
		m.add(p1);
		return m;
	}
	

	void update() {
		if ( isMoving ) {
			pos = dest.get();
			pos.mult( float(t)/float(steps) );
			myAngle = (finA-fromAngle)/float(steps)*float(t)+fromAngle;
			if (forward) {
				t++;
			}
			else
			{
				t--;
			}
				
			if ( t>steps ) {
				t = steps;
				forward = false;
				pos = dest.get();
				myAngle = finA;
				isMoving = false;
			}
			if ( t<0 ) {
				t = 0;
				forward = true;
				pos.set(0,0);
				myAngle = fromAngle;
				isMoving = false;
			}
		}
	}
	
	void reverse() {
		if (forward) {
			forward = false;
		}
		else
		{
			forward = true;
		}
		isMoving = true;
	}
	
	void draw() {
		pushMatrix();
		translate( pos.x , pos.y );
		rotate(myAngle);
		triangle( p1.x , p1.y , p2.x , p2.y , p3.x , p3.y );
		popMatrix();
	}
}	
class TriangleMesh {
	ArrayList<Triangle> m;
	PVector pos;


	TriangleMesh( PVector p ) {
		pos = p.get();
		m = new ArrayList<Triangle>();
	}

	void add( Triangle tri ) {
		m.add( tri );
	}

	void update() {
		for( Triangle tri : m ) {
			tri.update();
		}
	}
	
	boolean isMoving() {
		boolean move = false;
		for( Triangle tri : m ) {
			if (tri.isMoving) {
				move = true;
			}
		}
		return move;
	}
	
	void startMoving() {
		for( Triangle tri : m ) {
			tri.isMoving = true;
		}
	}
	
	void reverse() {
		for( Triangle tri : m ) {
			tri.reverse();
		}
	}
	
	void leftSplit( TriangleMesh c , int s) {
		PVector mid;
		Triangle t;
		PVector d = new PVector(-200,0);
		boolean flip = false;
		for( Triangle tri : c.m ) {
			mid = tri.mid31();
			t = new Triangle(  tri.p2 , tri.p3, mid , tri.fromAngle );
			if (flip) {
				t.setDest( d , PI , s );
				flip = false;
				d = t.p3dest().get();
				}
			else
				{
				t.setDest( d , 0 , s );
				flip = true;
				d = t.p2dest().get();
				}
				
				
			m.add( t );
		}
	}
	
	void rightSplit( TriangleMesh c , int s) {
		PVector mid;
		Triangle t;
		PVector d = new PVector(200,0);
		boolean flip = false;
		
		for( Triangle tri : c.m ) {
			mid = tri.mid31();

			t = new Triangle( tri.p1 , tri.p2 , mid , tri.fromAngle );
			if (flip) {
				t.setDest( d.get() , PI , s );
				flip = false;
				d = t.p3dest().get();
	
				}
			else
				{
				t.setDest( d.get() , 0 , s );
				flip = true;
				d = t.p1dest().get();
				}
			m.add( t );
		}
	}


	void draw() {
		pushMatrix();
		translate( pos.x , pos.y );
		for( Triangle tri : m ) {
			tri.draw();
		}
		popMatrix();
	}
}


