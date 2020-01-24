import fisica.*;

FWorld world;
int t;

void setup() {
  size(800, 600);
  Fisica.init(this);
  world = new FWorld();
  world.setEdges();
  FCircle b = new FCircle(10);
  b.setPosition(width/2,400);
  b.setDensity(0);
  world.add(b);
  t = 0;
  }

void draw() {
  background( 142 );
  if(t==0) {
      FCircle c = new FCircle(30);
      c.setPosition(width/2,20);
      c.setRestitution(0.5);
      
      world.add(c);

      t=100;
  }
  t--;
  world.step();
  world.draw();
  
}

