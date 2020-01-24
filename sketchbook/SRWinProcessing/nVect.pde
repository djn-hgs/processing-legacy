class nVect {
  float x,y,z;
  float[] coords;
  int dim;
  
  nVect( int d ) {
    dim = d;
    coords = new float[dim];
    for( int i = 0 ; i<dim ; i++ ) {
      coords[i] = 0;
    }
      x = 0;
      y = 0;
      z = 0;
  }
  
  nVect( nVect v ) {
    dim = v.dim;
    coords = new float[dim];
    for( int i = 0 ; i<dim ; i++ ) {
      coords[i] = v.coords[i];
    }
    x = coords[0];
    if (dim<2) {
      y = 0;
    }
    else {
    y = coords[1];
    }
    if (dim<3) {
      z = 0;
    }
    else {
      z = coords[2];
    }
  }
    
  
  void update() {
    for( int i = 0 ; i<dim ; i++ ) {
      if (random(-1,1)<0) {
        coords[i]--;
      }
      else {
        coords[i]++;
      }
    }
    x = coords[0];
    if (dim<2) {
      y = 0;
    }
    else {
    y = coords[1];
    }
    if (dim<3) {
      z = 0;
    }
    else {
      z = coords[2];
    }
  }
  
  float len() {
    float l = 0;
    for( int i = 0 ; i<dim ; i++ ) {
      l = l + coords[i]*coords[i];
    }
    return l;
  }
}  
  
