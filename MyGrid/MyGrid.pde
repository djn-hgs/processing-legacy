Grid myGrid;

void setup() {
  size(800,600,P3D);
  frameRate(200);
  background(0);
  stroke(255);
  noFill();
  myGrid = new Grid(new PVector(width/2,height/2,-100),new PVector(200,200,200),5+int(random(10)),5+int(random(10)),5+int(random(10)));
  myGrid.display();
  
}

void draw() {
  background(0);
  camera(mouseX, mouseY, (height/2) / tan(PI/6), mouseX, mouseY, 0, 0, 1, 0);
  myGrid.display();
  myGrid.update();
  frame.setTitle(int(frameRate) + " fps");

  
}
  
