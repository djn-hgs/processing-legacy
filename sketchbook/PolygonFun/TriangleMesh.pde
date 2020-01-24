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

