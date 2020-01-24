ArrayList<Snowflake> sf;
int counter;

void setup() {
  frameRate(30);
  size(800,600);
  background(0);
  counter = 0;
  sf = new ArrayList<Snowflake>();
  sf.add(new Snowflake(new PVector(400,0),new PVector(0,2), 20.0, PI/100,3));
}

void draw() {
    fill(0,80);
    rect(0,0,width,height);
  for (int i=sf.size()-1;i >= 0;i--) {
    Snowflake s = sf.get(i);
    s.display();
    s.update();
    if ( s.issettled() ) {
      sf.remove(i);
    }
  }
  counter++;
  if (counter>9) {
    sf.add(new Snowflake(new PVector(random(width),0),new PVector(0,5+random(5)), random(20), PI/100,int(random(5))));
    counter = 0;
  }
}

void mouseClicked() {
  
}
