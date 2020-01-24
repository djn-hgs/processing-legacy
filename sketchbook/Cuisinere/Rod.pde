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

