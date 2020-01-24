class Sample {
  ArrayList<Float> values;
  Distribution dist;
  int size;
  
  Sample( int n , Distribution d ) {
    values = new ArrayList<Float>();
    dist = d;
    size = n;
    for(int i = 0 ; i<n ; i++) {
      values.add( d.get() );
    }
  }

  float mean() {
    float sum = 0;
    for( float v : values ) {
      sum = sum + v;
    }
    return sum/float(values.size());
  }
  
  float stDev() {
    float sumSq = 0;
    for( float v : values ) {
      sumSq = sumSq + v * v;
    }
    float m = mean();
    return ( sumSq  - size * m * m ) / ( size - 1 );
  }
  
  float normalize() {
    return ( mean() - dist.mu ) / ( dist.sigma / sqrt( size ) );
  }
  
  float tStat() {
    return ( mean() - dist.mu ) / ( stDev() / sqrt( size ) );
  }
  
}