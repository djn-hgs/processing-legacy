class SurfaceGrid {
  PShape s;
  PShape m;
  int cols;
  int rows;
  float sWidth;
  float sHeight;
  float hStep;
  float vStep;
  hGrid grid;

  SurfaceGrid( float w, float h, hGrid g ) {
    grid = g;
    rows = grid.gHeight;
    cols = grid.gWidth;
    sWidth = w;
    sHeight = h;
    hStep = sWidth/cols;
    vStep = sHeight/rows;
    m = createShape(GROUP);
    s = createShape();
    build();
  }

  void build() {
    m = createShape(GROUP);

    for (int i = 1 ; i<cols ; i++ ) {
      for ( int j=1 ; j<rows ; j++ ) {
        s = createShape();

        s.beginShape();
        s.noFill();
        s.stroke(0);
        s.vertex((i-1)*hStep, grid.get(i-1, j-1), (j-1)*vStep);
        s.vertex((i-1)*hStep, grid.get(i-1, j), j*vStep);
        s.vertex(i*hStep, grid.get(i, j), j*vStep);
        s.vertex(i*hStep, grid.get(i, j-1), (j-1)*vStep);
        s.endShape(CLOSE);
        m.addChild(s);
      }
    }
  }

  void display() {
    pushMatrix();
    translate( -sWidth/2, 0, -sHeight/2);
    shape(m);
    popMatrix();
  }
}

