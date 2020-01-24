int x;
int y;
int r;
int rs;
int iter;


void setup() {
  size( 400 , 300 );
  noFill( );
  smooth();
  stroke( 40 , 0 , 0 );
  strokeWeight( 1 );
  frameRate( 30 );
  x = 0;
  y = 0;
  r = 10;
  iter = 1;
  }

void draw() {
    background( 240 );
    int c = 0;
    int h = width / r;
    int v = height / r;
    for( int i = 0 ; i <= h ; i++ ) {
      for( int j = 0 ; j <= v ; j++ ) {
        if ( c % iter == 0 ) {
          fill( 40 );
          rs = 2 *r;
        }
        else {
          noFill();
          rs = r;
        }
        ellipse( i * r + r / 2 , j * r + r / 2 , rs , rs );
        c++;
      }
    }
    iter++;
}