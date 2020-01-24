class Uniform extends Distribution {
  float uniLeft;
  float uniRight;
  
  Uniform( float l , float r ) {
    uniLeft = l;
    uniRight = r;
  }
  
  Uniform() {
    this(0,1);
  }
  
  float get() {
    return random(uniLeft,uniRight);
  }
}
