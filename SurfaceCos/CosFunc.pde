class CosFunc extends TrigFunc {

  CosFunc( int h, int v, float a ) {
    super( h , v , a );
  }

  float getValue( float x, float y ) {
    return amp*cos(hf*x*TWO_PI)*cos(vf*y*TWO_PI);
  }

}

