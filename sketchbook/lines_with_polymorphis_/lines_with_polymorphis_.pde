ArrayList< Shape > shapes;

int n = 100;
int s = 10;

void setup() {
  
    size(400, 200); 
      
    smooth();
    
    frameRate(30);
   
    shapes = new ArrayList< Shape >();

}

void draw() {  
    background( 0 );  
  
    if ( s < n ) {
            shapes.add( new Line() );
          s++;
    }
        
    for ( Shape l : shapes ) {
        l.display();
        l.update();
    }

    for ( int i = shapes.size() - 1 ; i >= 0 ; i--) {
        Shape l = shapes.get( i );
        if ( l.isDone() ) {
          shapes.remove( i );
           s--;
        }
        
    }

}

// A simple shape class

class Shape {
  color col;
  int life;
  int lifeCount;
  
  Shape() {}
  
  void display() {}
  
  void update() {
      if ( life > 0 ) {
        life--;
      }
    }
    
    boolean isDone() {
      return ( life == 0 );
    }
}

// And now lines

class Line extends Shape {
    PVector start;
    PVector finish;
  
    
    
    Line( PVector s , PVector f , int c , int l ) {
        start = s.get();
        finish = f.get();
        col = c;
        life = l;
        lifeCount = l;
    }
    
    Line() {
        this( new PVector( random( 0 , width ) , 0 ) , new PVector( random( 0 , width ) , height ) , color( random( 255 ) ) , 10 + int( random( 100 ) ) ) ;
    }
    
   
    void display() {
        stroke( col , 255 * life / lifeCount );
        strokeWeight(12 * life / lifeCount );
        line( start.x , start.y , finish.x , finish.y );        
    }
    
}
    