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

