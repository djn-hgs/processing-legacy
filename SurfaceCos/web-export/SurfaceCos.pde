import peasy.*;
PeasyCam cam;

SurfaceGrid s;
hGrid c;

void setup() {

  size( 800, 600, P3D );
  cam = new PeasyCam( this, 0, 0, 0, 500);
  cam.rotateX(PI/6);

  c = new CosGrid( 2 , 3, 10 , 10 , 100 );

  s = new SurfaceGrid( 200, 200, c );
//  s.build();
}

void draw() {
  background(240);
  s.display();
}

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


