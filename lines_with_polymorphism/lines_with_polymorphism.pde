ArrayList< Shape > shapes;

int n = 100;
int s = 10;
Shape ns;
            

void setup() {
  
    size(800, 600); 
      
    smooth();
    
    frameRate(30);
   
    shapes = new ArrayList< Shape >();

}

void draw() {
    background( 0 );  
  
    if ( s < n ) {
            int choice = ( int ) random( 2 );
            switch( choice ) {
              case 0:
              ns = new Circle();
              break;
              case 1:
              ns = new Line();
              break;
            }
            
            shapes.add( ns );
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
    
    float ageMult( float v ) {
      return v * life / lifeCount;
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
        stroke( col , ageMult( 255 ) );
        strokeWeight( ageMult( 12 ) );
        line( start.x , start.y , finish.x , finish.y );        
    }
    
}

// And now circles

class Circle extends Shape {
  PVector centre;
  float radius;
  
  Circle( PVector c , float r , int cl , int l ) {
    centre = c.get();
    radius =r;
    col = cl;
    life =l;
    lifeCount = l;
  }
  
  Circle() {
    this( new PVector( random( width ) , random( height ) ) , 10 + random( 100 ) , color( random( 255 ) ) , 10 + int( random( 100 ) ) ) ;
  }
  
  void display() {
        fill( col , ageMult( 255 ) );
        noStroke();
        ellipse( centre.x , centre.y , ageMult( radius ) , ageMult( radius ) );
  }
}