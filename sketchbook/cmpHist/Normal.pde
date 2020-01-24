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
  
  float randomGaussian() {
    float x1, x2, y1, y2;
    x1 = random(1);
    x2 = random(2);
    y1 = sqrt( -2 * (log(x1))) * cos(2  * PI * x2);
    y2 = sqrt( -2 * (log(x1))) * sin(2  * PI * x2);
    return y1;
  }
}

