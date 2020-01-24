float xmin = 3;
float xmax = 4;
float ymin = 0;
float ymax = 1;

int iterations = 10;
float show = 0.1;
int lastFew;
int i;

void setup() {
  size( 800, 600 );
  background( 255 );
  noSmooth();
  i = 0;
  lastFew = int( iterations * show );
}

void draw() {

  stroke( 255 );

  line( i, 0, i, height );

  for ( int j = 0; j < height; j ++ ) {
    stroke( 0 );
    float a = xTrans( i );
    float y = 0.4;
    for ( int k = 0; k < iterations; k++ ) {
      y = a * y * ( 1 - y );
      if ( k >= iterations - lastFew ) {
        point( i, yPixTrans( y ) );
      }
    }
  }
  i++;
  if ( i >= width ) {
    i = 0;
    iterations = iterations * 10;
    lastFew = int( iterations * show );
  }
}

float f( float x ) {
  return sin(x);
}

float xTrans( int i ) {
  return xmin + ( xmax - xmin ) * i / width;
}

float yTrans( int j ) {
  return ymin + ( ymax - ymin ) * j / height;
}

int xPixTrans( float x ) {
  return int( width * ( x - xmin ) / ( xmax - xmin ) );
}

int yPixTrans( float y ) {
  return int( height - height * ( y - ymin ) / ( ymax - ymin ) );
}