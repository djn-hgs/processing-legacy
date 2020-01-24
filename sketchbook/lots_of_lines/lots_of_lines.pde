ArrayList< Line > lines;

int n = 100;
int s = 10;

void setup() {
  
    size(400, 200); 
      
    smooth();
    
    frameRate(30);
   
    lines = new ArrayList< Line >();

}

void draw() {  
    background( 0 );  
  
    if ( s < n ) {
            lines.add( new Line() );
          s++;
    }
        
    for ( Line l : lines ) {
        l.display();
        l.update();
    }

    for ( int i = lines.size() - 1 ; i >= 0 ; i--) {
        Line l = lines.get( i );
        if ( l.isDone() ) {
          lines.remove( i );
           s--;
        }
        
    }

}

// A simple Line class

class Line {
    PVector start;
    PVector finish;
    color col;
    int life;
    int lifeCount;
    
    
    
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
    
    void update() {
      if ( life > 0 ) {
        life--;
      }
    }
    
    void display() {
        stroke( col , 255 * life / lifeCount );
        strokeWeight(12 * life / lifeCount );
        line( start.x , start.y , finish.x , finish.y );        
    }
    
    boolean isDone() {
      return ( life == 0 );
    }
}
    