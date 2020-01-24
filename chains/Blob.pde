class Blob {
  PVector pos, vel; // Position and velocity
  PVector weight ;
  Boolean fixed;
  float radius = 10; // radius
  float mass; // mass
  float gravity; // gravity
  float damp; // damping

    Blob(float x, float y, float m, float g, float d) {
    pos = new PVector(x, y);
    vel = new PVector(0, 0);
    fixed = false;
    mass = m;
    gravity = g;
    damp = d;
    weight = new PVector(0, -m*g);
  }

  void fix() {
    fixed = true;
  }

  void unfix() {
    fixed = false;
  }

  void update(float dt) {
    if ( ! fixed ) {
      pos.add(PVector.mult(vel, dt));
      vel.add(PVector.mult(weight, dt));
      vel.mult(1-damp);
    }
  }
  
  void setVel(PVector v) {
    vel.add(v );
  }
  

  void draw() {
    //    noStroke();
    //    ellipse(pos.x, pos.y, radius*2, radius*2);
  }

  void applyImpulse(PVector f, float dt) {
    if ( ! fixed ) {
      PVector i=new PVector();
      i= f.get();
      i.mult(dt/mass);
      vel.add(i);
    }
  }
}

