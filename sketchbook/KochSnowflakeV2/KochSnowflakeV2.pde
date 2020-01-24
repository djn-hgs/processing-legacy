Snowflake sf;

void setup() {
  size(800,600);
  background(0);
  sf = new Snowflake(new PVector(40,440), new PVector(600,440), new PVector(320,40));
  sf.display();
  sf.grow();
  sf.display();
}

 
