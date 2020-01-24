int ox, nx;
int oy, ny;
int w = 10;
int h = 2;
float sf;

ArrayList<Blob> b;

Chain c;

void setup() {
  size(640, 480);
  b = new ArrayList<Blob>();
  float gw=10;
  float gh=5;
  float xl=-gw/2;
  float xr=gw/2;
  float yh=0;
  float yl=-gh;
  sf = width/gw/2;
  float g= -10; // uniform gravity
  float a = float(w*h); // number of cells
  float s= 10/a; // uniform stiffness
  float m= 1/a; // uniform mass
  float d = 0.1; // uniform damping
  float l= gw/w; // uniform natural length
  for (int j = 0; j < h; j++ ) {
    for (int i = 0; i < w; i++ ) {

      Blob bn = new Blob(xl+i*gw/w, yl+j*gh/h, m, g, d);
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
  translate(width/2, height/2);
  scale(sf);
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
  if ( mousePressed ) {
    float vx = (nx - ox)/sf;
    float vy = (ny - oy)/sf;
    PVector v = new PVector(vx, vy);
    b.get((h-1)*w).applyImpulse(v,dt);
  }
}

