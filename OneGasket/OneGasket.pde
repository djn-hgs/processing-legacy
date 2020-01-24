Gasket gasket;

void setup() {
  size(640, 480);
  gasket = new Gasket(new PVector(40,440),new PVector(600,440), new PVector(320,40));
}

void draw() {
  background(0);
      gasket.display();
} 

void mousePressed() {
      gasket.grow();
  
}
