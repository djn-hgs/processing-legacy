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
	
	void reverse() {
		for( Triangle tri : m ) {
			tri.reverse();
		}
	}
	void perp( TriangleMesh c ) {
		PVector mid;
		Triangle t1 , t2;
		float d1x = -200;
		float d2x = 200;
		float d1y = 0;
		float d2y = 0;
		boolean flip1 = false;
		boolean flip2 = false;
		for( Triangle tri : c.m ) {
			mid = tri.mid31();
			t2 = new Triangle(  tri.p2 , tri.p3, mid , tri.fromAngle );
			if (flip2) {
				t2.setDest( new PVector( d2x , d2y ) , PI , 500 );
				flip2 = false;
				d2x = t2.p3dest().x;
				d2y = t2.p3dest().y;
				
				}
			else
				{
				t2.setDest( new PVector( d2x , d2y ) , 0 , 500 );
				flip2 = true;
				d2x = t2.p2dest().x;
				d2y = t2.p2dest().y;
				}
				
				
			m.add( t2 );
			t1 = new Triangle( tri.p1 , tri.p2 , mid , tri.fromAngle );
			if (flip1) {
				t1.setDest( new PVector( d1x , d1y ) , PI , 100 );
				flip1 = false;
				d1x = t1.p3dest().x;
				d1y = t1.p3dest().y;
				
				}
			else
				{
				t1.setDest( new PVector( d1x , d1y ) , 0 , 100 );
				flip1 = true;
				d1x = t1.p1dest().x;
				d1y = t1.p1dest().y;
				}
			m.add( t1 );
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
