ArrayList<Gasket> gaskets;

void setup() {
  size(640, 800);
  gaskets = new ArrayList<Gasket>();
  gaskets.add(new Gasket(new PVector(40,440),new PVector(600,440), new PVector(320,40))); 
}

void draw() {
  background(0);
  for (Gasket g: gaskets) {
      g.display();
    }    
}

void mousePressed() {
  for (Gasket g: gaskets) {
      g.grow();
  }    
  
}

