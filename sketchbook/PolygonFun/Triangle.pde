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