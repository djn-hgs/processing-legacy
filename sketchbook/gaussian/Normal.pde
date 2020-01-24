class Normal extends Distribution {
  float mu;
  float sigma;
  
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

