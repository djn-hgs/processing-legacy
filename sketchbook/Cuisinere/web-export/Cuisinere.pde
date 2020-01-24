RodSet myRods;
Rod r;
Rod objectRod;
Rod targetRod;
Boolean waitingForRod = true;
Boolean rodSelected = false;
Boolean keyDown = false;



void setup() {
  frameRate(60);
  size(1024, 640);
  myRods = new RodSet();
  r = new Rod( 7 , new PVector( 512 , 320 ) );
  r.target( new PVector( random(width), random(height) ) );
  myRods.add( r );
  }

void draw() {
  background( 51 );
  myRods.display();
  myRods.update();
  
}

void mouseClicked() {
}

void mousePressed() 
{ 
  if (mouseButton == LEFT)  // left button
  {
    if (mouseEvent.getClickCount()==2) {  // double-click
      println("double-click");
      
    }
    else {
      println( "left-click" );
      // r = myRods.closestRod( float(mouseX) , float(mouseY) );
      r = myRods.clicked( float(mouseX) , float(mouseY) );
      if ( !(r == null) ) {
        if (waitingForRod) {
          objectRod = r;
          objectRod.isSelected = true;
          objectRod.showCells = true;
          waitingForRod = false;
        }
        else if (!waitingForRod) {
          targetRod = r;
          targetRod.isSelected = false;
          objectRod.target( targetRod.corner() );
          waitingForRod = true;
        }
      }
      else
      {
        waitingForRod = true;
        if (!(objectRod == null)) {
        objectRod.isSelected = false;
        objectRod.showCells = false;
        }
      }
      }  
  }
  else if (mouseButton == RIGHT) // right button
  {
    println("right");
    
    myRods.add( new Rod( int( random(10) + 1 ) , new PVector( mouseX , mouseY ) ) );      
  }
}

void keyPressed() {
  int myKey = int(key);
  println("pressed " + int(key) + " " + keyCode);
  if ( ( myKey > 47 ) && ( myKey < 58 ) ) {
    int value = myKey - 48;
    if ( value == 0 ) {
      value = 10;
    }
    
    if (!waitingForRod) {
      objectRod.isSelected = false;
      objectRod.showCells = false;
    }

    Rod r = new Rod( value , new PVector( mouseX , mouseY ) );
    objectRod = r;
    objectRod.isSelected = true;
    objectRod.showCells = true;
    waitingForRod = false;
    myRods.add( r );
    
  }    
  
}

void keyTyped() {
  println("typed " + int(key) + " " + keyCode);
}

void keyReleased() {
  println("released " + int(key) + " " + keyCode);
}

class Path {
  PVector start;
  PVector pos;
  PVector finish;
  PVector vel;
  float len;
  float par;
  float spd;
  Boolean isdone;
  
  Path( PVector s , PVector f , float l , float v ) {
    start = s.get();
    finish = f.get();
    
    len = l;
    par = 0;
    spd = v;
    
    pos = start.get();
    vel = finish.get();
    vel.sub(start);
    vel.mult( v/l );
    
    isdone = false;
  }
  
  void update() {
    if (!isdone) {
      par = par + spd;
      if ( par >= len ) {
        pos = finish.get();
        isdone = true;
      }
      else {
        pos.add( vel );
      }
    }
  }
}
class Rod {
  Boolean isSelected;
  PVector pos;
  PVector target;
  Path path;
  int value;
  int fsize;
  float cwidth;
  float cheight;
  Boolean ismoving;
  Boolean showCells;
  color[] palette = { color(245, 52, 52), 
                      color(250, 250, 250),
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) ,
                      color(149, 255, 63) };

      
  Rod( int v , PVector p ) {
    pos = p.get();
    value = v;
    fsize = 12;
    cwidth = 50;
    cheight = 50;
    ismoving = false;
    isSelected = false;
    showCells = false;
   }
  
  void display() {
    textSize(fsize);
    pushMatrix();
    if (ismoving) {
      translate( path.pos.x , path.pos.y );
    }
    else
    {
      translate(pos.x,pos.y);
    }
    fill(palette[value-1]);
    if (isSelected) {
      strokeWeight(5);
    }
    else {
      strokeWeight(2);
    }
    rect( 0.0 , 0.0 , value*cwidth , cheight , 5 );
    if ( showCells ) {
      for ( int i = 1 ; i < value ; i++ ) {
        strokeWeight( 1 );
        line( i*cwidth , 0 , i*cwidth , cheight );
      }
    }
    popMatrix();
    
  }
  
  void target( PVector t ) {
    target = t.get();
    ismoving = true;
    path = new Path( pos , target , 1.0 , 0.01 );
  }
  
  void update() {
    if (ismoving) {
      path.update();
      pos = path.pos.get();
      if (path.isdone) {
        ismoving = false;
        pos = target.get();
        isSelected = false;
        showCells = false;
      }
    }
  }
  
  float dist( float x , float y ) {
    return (( pos.x - x )*( pos.x - x ) + ( pos.y - y )*( pos.y - y ) );
  }
  
  Boolean clicked( float x , float y ) {
    if ( (x > pos.x) && (y > pos.y) && (x < pos.x + value*cwidth ) && (y < pos.y + cheight ) ) {
      return true;
    }
    else {
      return false;
    }
  }
  
  PVector corner() {
    PVector c = pos.get();
    c.add( new PVector( value*cwidth , 0 ) );
    return c;
  }
}

class RodSet {
  ArrayList<Rod> rods;

  RodSet() {
     rods = new ArrayList<Rod>();
  }
    
  void display() {
    for ( Rod r: rods ) {
      r.display();
    }
  }

  void update() {
    for ( Rod r: rods ) {
      r.update();
    }
  }
  
  void add(Rod r) {
    rods.add( r );
  }
  
  Rod closestRod( float x , float y ) {
    Rod bestRod = null;
    Boolean foundBest = false;
    float distSq = 0;
    for ( Rod r: rods ) {
      float rdistSq = r.dist( x , y );
      if (( rdistSq < distSq ) || (!foundBest)) {
        distSq = rdistSq;
        foundBest = true;
        bestRod = r;
      }
    }
  return bestRod;    
      
  }
  
  Rod clicked( float x , float y ) {
    Rod clickedRod = null;
    Boolean foundClicked = false;
    for ( Rod r: rods ) {
      if ( r.clicked( x , y ) ) {
        clickedRod = r;
        foundClicked = true;
      }
    }
  return clickedRod;
  }
  
}

