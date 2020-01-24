class hGrid {
  int gWidth;
  int gHeight;

  Float[][] hData;

  hGrid() {
    hData  = new Float[0][0];
    gHeight = 1;
    gWidth = 1;
  }


  hGrid( Float[][] h ) {
    hData  = h;
    gHeight = h.length;
    gWidth = h[0].length;
  }

  float get( int i, int j ) {
    return hData[i][j];
  }
}

