int n = 15;
float[][] points = new float[ n ][ 2 ];
int minIndex;

void setup() {
  size( 640 , 480 );
  background( 40 );
  for( int i = 0 ; i < n ; i ++ ) {
    points[ i ][ 0 ] = random( width );
    points[ i ][ 1 ] = random( height );
  }
  minIndex = getMin( points );
}

void draw() {
  float x0 , y0 , x1 , y1;
  int lastIndex = minIndex;
  int[][] edges = new int[ 1 ][ 2 ]; 
  showPoints( points , minIndex );
  for ( int i = 0 ; i < n ; i ++ ) {
    if ( ! ( i == lastIndex ) ) {
      line( points[ lastIndex ][ 0 ] , points[ lastIndex ][ 1 ] , points[ i ][ 0 ] , points[ i ][ 1 ] );
    }
  }
}

int getMin( float[][] p ) {
  float x_min = p[ 0 ][ 0 ];
  int minIndex = 0;
  
  for( int i = 1 ; i < p.length ; i++ ) {
    if ( p[ i ][ 0 ] < x_min ) {
      x_min = p[ i ][ 0 ];
      minIndex = i;
    }
  }
  
  return minIndex;
}

void showPoints( float[][] p , int minIndex ) {
  for( int i = 0 ; i < n ; i ++ ) {
    stroke( 200 );
    fill( 200 );
    if ( i == minIndex ) {
      stroke( 200 , 0 , 0 );
      fill( 200 , 0 , 0 );
    }
      ellipse( p[ i ][ 0 ] , p[ i ][ 1 ] , 6 , 6 );
  }
}