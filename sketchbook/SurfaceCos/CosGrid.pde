class CosGrid extends hGrid {
  int hf;
  int vf;

  float amp;

  CosGrid( int h, int v, int gw, int gh, float amp ) {
    hf = h;
    vf = v;
    gWidth = gw;
    gHeight = gh;
    hData = new Float[gHeight][gWidth];
    for (int i = 0 ; i<gWidth ; i++ ) {
      for ( int j=0 ; j<gHeight ; j++ ) {
        hData[i][j] = amp*cos(float(i*hf)/float(gWidth)*TWO_PI)*cos(float(j*vf)/float(gHeight)*TWO_PI);
      }
    }
  }
}

