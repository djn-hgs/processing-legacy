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