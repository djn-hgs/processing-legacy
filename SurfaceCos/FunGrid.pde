class FunGrid extends HeightGrid {
  HeightFunc func;

  FunGrid( int gw, int gh, HeightFunc f ) {
    gWidth = gw;
    gHeight = gh;
    func = f;
    hData = new Float[gWidth][gHeight];
    build();
  }

  void build() {

    for (int i = 0 ; i<gWidth ; i++ ) {
      for ( int j=0 ; j<gHeight ; j++ ) {
        hData[i][j] = func.getValue(float(i)/float(gWidth), float(j)/float(gHeight));
      }
    }
  }

  float max() {
    return func.max();
  }
  
  float min() {
    return func.min();
  }
  
  
}

