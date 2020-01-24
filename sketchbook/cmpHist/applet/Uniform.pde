class Uniform extends Distribution {
  float uniLeft;
  float uniRight;
  
  Uniform( float l , float r ) {
    uniLeft = l;
    uniRight = r;
    mu = ( l + r ) / 2;
    sigma = ( r - l )*( r - l ) / 12;
  }
  
  Uniform() {
    this(0,1);
  }
  
  float get() {
    return random(uniLeft,uniRight);
  }
}
