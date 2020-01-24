import fisica.*;

FWorld world;
FCircle c;
int t;
int n;

void setup() {
  size(800, 600);
  Fisica.init(this);
  world = new FWorld();
  world.setEdges();
  n=8;
  createPegs(n,300,200);
  createBins(2*n);
  t = 0;
}

void draw() {
  background(140);
  if (t==0) {
    addBall();
    t=100;
  }
  t--;
  world.step();
  world.draw();
}

void createPegs(int r,float w, float h) {
  float xSep = w/r;
  float ySep = h/r;
  float xPos = width/2;
  float yPos = (450-h)/2;
  FCircle d;
  for(int i=0;i<r;i++) {
    for(int j=0;j<=i;j++) {
      
    d = new FCircle(5);
    d.setPosition(xPos+j*xSep-i*xSep/2,yPos+i*ySep);
    d.setRestitution(0);
    d.setStatic(true);
    d.setName("peg");
    world.add(d);
    }
  }
}

void createBins(int b) {
  float xSep = width/b;
  FPoly myBin;
  for(int i=0;i<b;i++) {
    myBin = new FPoly();
    myBin.vertex(0,-150);
    myBin.vertex(0,0);
    myBin.vertex(2,0);
    myBin.vertex(2,-150);
 
    myBin.setStatic(true);
    myBin.setPosition(xSep*i,height);
    world.add(myBin);
    
  }
}


void addBall() {
  c = new FCircle(20);
  c.setPosition(width/2,0);
  c.setRestitution(0.1);
  c.setName("ball");
  world.add(c);
}

void contactStarted(FContact contact) {
  FBody b1,b2;
  float cV;
  float v = 10;
  b1 = contact.getBody1();
  b2 = contact.getBody2();
    if (b1.getX()==b2.getX()) {  
    if((b1.getName()=="ball") && (b2.getName()=="peg")) {
      if(b1.getVelocityX()==0) {
      if(random(2)<1) {
        b1.adjustVelocity(v,0.0);
      }
      else {
        b1.adjustVelocity(-v,0.0);
      }
      }
    }
    if((b2.getName()=="ball") && (b1.getName()=="peg")) {
      if(b2.getVelocityX()==0) {
      if(random(2)<1) {
        b2.adjustVelocity(v,0.0);
      }
      else {
        b2.adjustVelocity(-v,0.0);
      }
    }
    }
    }
  }

