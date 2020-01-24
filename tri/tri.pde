float[] x;
float[] y;


void setup() {
  float[] x = new float[ 3 ];
  float[] y = new float[ 3 ];
  
  size( 640 , 480 );
  for( int i = 0 ; i < 3 ; i++ ) {
    x[ i ] = random( width );
    y[ i ] = random( height );
  }
}

void draw() {
  int j;
  background( 0 );
  for( int i = 0 ; i < 3 ; i++ ) {
    j = ( i + 1 ) % 3;
    line( x[ i ] , y[ i ] , x[ j ] , y[ j ] );
  }
}