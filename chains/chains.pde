int ox, nx;
int oy, ny;
int w = 20;
int h = 10;

ArrayList<Blob> b;

Chain c;

void setup() {
  size(640, 480, P3D);
  b = new ArrayList<Blob>();
  float xl=-width/4;
  float xr=width/4;
  float yh=0;
  float yl=-height/2;
  float g= -10; // uniform gravity
  float a = float(w*h); // number of cells
  float s= 1000000/a; // uniform stiffness
  float m= 1000/a; // uniform mass
  float d = 0.1; // uniform damping
  float l= (xr-xl)/(w); // uniform natural length
  for (int j = 0; j < h; j++ ) {
    for (int i = 0; i < w; i++ ) {

      Blob bn = new Blob(xl+i*(xr-xl)/w, yl+j*(yh-yl)/h, m, g, d);
      b.add(bn);
    }
  }

  b.get(0).fix();
  b.get(w-1).fix();

  c=new Chain();

  for (int j = 0; j<h; j++ ) {
    for (int i = 1; i<w; i++ ) {
      c.add(new Link(l, s, b.get(j*w+i-1), b.get(j*w+i)));
    }
  }
  for (int i = 0; i<w; i++ ) {
    for (int j = 1; j<h; j++ ) {
      c.add(new Link(l, s, b.get((j-1)*w+i), b.get(j*w+i)));
    }
  }
}

void draw() {
  background(0);
  float  dt = 1/frameRate;
  lights();
  translate(width/2, height/2);
  for (Blob bn : b ) {
    bn.draw();
  }
  c.draw();
  for (Blob bn : b ) {
    bn.update(dt);
  }
  c.update(dt);
  ox = nx;
  oy = ny;
  nx = mouseX;
  ny = mouseY;
  if ( ! ((ox == nx) ^ (oy == ny)) ) {
    float vx = (nx - ox)/dt;
    float vy = (ny - oy)/dt;
    PVector v = new PVector(vx, vy);
      b.get((h-1)*w).setVel(v);
  }
}

