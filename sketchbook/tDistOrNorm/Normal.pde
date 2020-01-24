class Normal extends Distribution {
  
  Normal( float m , float s ) {
    mu = m;
    sigma = s;
  }
  
  Normal() {
    this(0,1);
  }
  
  float get() {
    return randomGaussian()*sigma+mu;
  }
}