class Chain {
  ArrayList<Link> links;

  Chain() {
    links = new ArrayList<Link>();
  }

  void add(Link l) {
    links.add( l );
  }

  void draw() {
    for (Link l : links) {
      l.draw();
    }
  }

  void update(float dt) {
    PVector d;
    for (Link l : links) {
      d = PVector.sub(l.b2.pos, l.b1.pos);
      d.setMag(l.force());
      l.b1.applyImpulse(d, dt);
      d.mult(-1);
      l.b2.applyImpulse(d, dt);
      
    }
  }
}

