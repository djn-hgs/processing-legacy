class numLine {
  float left;
  float right;
  float step;
  float dist;
  int count;
  ArrayList< post > posts;

  numLine( float l, float r, float s, float d ) {
    float pos;
    left = l;
    right = r;
    step = s;
    dist = d;
    posts = new ArrayList< post >();
    count = int( ( right - left ) / step );

    for ( int i = 0 ; i <= count ; i++ ) {
      pos = left + i * step;
      posts.add( new post( new PVector( pos, dist, 0 ), step/5 ) );
    }
  }

  void display() {
    for ( post p : posts ) {
      p.display();
    }
  }
}

