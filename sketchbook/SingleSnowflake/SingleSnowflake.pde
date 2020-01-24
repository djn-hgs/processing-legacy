Snowflake sf;

void setup() {
  frameRate(30);
  size(800,600);
  background(0);
  sf = new Snowflake(new PVector(400,300),new PVector(0,0), 200.0, PI/1000,0);
}

void draw() {
    background(0);
    sf.display();
    sf.update();
}

void mouseClicked() {
  sf.grow();
}
