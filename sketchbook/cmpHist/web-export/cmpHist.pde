
Histogram h1;
normHist h2;
tHist  h3;
meanHist h4;
Distribution u;
Distribution n;
Demo myDemo;
color blue;
color red;
color green;
color purple;
boolean uniform;

void setup() {
  size(800, 600);
  blue = color( 0 ,0 , 255 , 80 );
  red = color( 255 , 0 , 0 , 10 );
  green = color( 0 , 255 , 0 , 100 );
  purple = color( 255 , 0 , 255 , 100 );
  h1 = new Histogram(100,600,600,400);
  h1.createBins(-3,3,20);
  h1.setColor(red);
  h2 = new normHist( 100 , 600 , 600 , 400 );
  h2.createBins(-3,3,20);
  h2.setColor(blue);
  h3 = new tHist( 100 , 600 , 600 , 400 );
  h3.createBins(-3,3,20);
  h3.setColor(green);
  h4 = new meanHist( 100 , 600 , 600 , 400 );
  h4.createBins(-3,3,20);
  h4.setColor(purple);
  u = new Uniform(-2,2);
  n = new Normal();
  uniform = true;
  myDemo = new Demo(u , h1 , 5);
  myDemo.add( h2 , 20 );
  myDemo.add( h3 , 20 );
  myDemo.add( h4 , 20 );

  
}

void draw() {
  background(140);
  myDemo.update();
  myDemo.draw();
}

void keyPressed() {
  if(key=='.') {
    myDemo.setSampleSize(myDemo.sampleSize+1);
  }
  if(key==',') {
    myDemo.setSampleSize(myDemo.sampleSize-1);
  }
  if(key == 'l' ) {
    myDemo.setRate(myDemo.rate+10);
  }
  if(key == ';' ) {
    myDemo.setRate(myDemo.rate-10);
  }
  if(key == ' ' ) {
    if(uniform) {
      uniform = false;
      myDemo.setDist( n );
    }
    else {
      uniform = true;
      myDemo.setDist( u );
    }
  }
  if ( key == 'd' ) {
    h1.flip();
  }
  if ( key == 'n' ) {
    h2.flip();
  }
  if ( key == 't' ) {
    h3.flip();
  }
  if ( key == 'm' ) {
    h4.flip();
  }
}  

class Ball {
  float xPos;
  float yPos;
  float xDest;
  float yDest;
  float xVel;
  float yVel;
  float xAcc;
  float yAcc;
  float size;
  float value;
  color myColor;
  boolean moving;
  
  Ball( float x, float y ,  float d , float v ) {
    xPos = x;
    yPos = y;
    xDest = xPos;
    yDest = d;
    xVel = 0;
    yVel = 0;
    xAcc = 0;
    yAcc = 0.1;
    size = 10;
    value = v;
    myColor = color(150);
    moving = true;
  }
  
  void setColor(color c) {
    myColor = c;
  }
  
  void setSize(float v) {
   size = v;
  } 
  
  float getValue() {
    return value;
  }
  
  void update() {
    if (moving) {
      xPos = xPos + xVel;
      yPos = yPos + yVel;
      xVel = xVel + xAcc;
      yVel = yVel + yAcc;
      if(yPos>=yDest-size) {
        yPos = yDest;
        moving = false;
      }
    }
  }
  
  boolean isDone() {
    if(moving) {
      return false;
    }
    else {
      return true;
    }
  }
    
  
  void draw() {
    fill(myColor);
    ellipse(xPos,yPos,size,size);
  }
}
class Bin {
  float binLeft;
  float binRight;
  int binCnt;
  boolean strictLeft;
  boolean strictRight;
  float xPos;
  float yPos;
  float binWidth;
  float binHeight;
  float binScale;
  color myColor;
  
  Bin( float x , float y , float w , float h , float l , float r ) {
    xPos = x;
    yPos = y;
    binWidth = w;
    binHeight = h;
    binLeft = l;
    binRight = r;
    binCnt = 0;
    binScale = 1;
    strictLeft = true;
    myColor = color(150);
    strictRight = false;
  }
  
  void setStrictLeft() {
    strictLeft = true;
  }
  
  void setStrictRight() {
    strictRight = true;
  }
  
  void setScale( float s ) {
    binScale = s;
  }
  
  void reZero() {
    binCnt = 0;
  }
  
  float midX() {
    return xPos+binWidth/2;
  }
  
  boolean contains( float v ) {
    boolean isIn = false;
    
    if( (v>binLeft) && (v<binRight) ) {
      isIn = true;
    }
  
    if( strictLeft && (v==binLeft) ) {
     isIn = true;
    }
   
    if( strictRight && (v==binRight) ) {
     isIn = true;
    }
   
    return isIn;
    
  }
  
  void increment() {
    binCnt++;
  }
  
  float get() {
    return binCnt/(binRight-binLeft);
  }
  
  void setColor(color c) {
    myColor = c;
  }
  
  void draw() {
    
    if ( (get() * binScale) > binHeight ) {
      println( get() + " and " + binScale + " and " + binHeight );
    }
    fill(myColor);
    rect(xPos,yPos,binWidth,-get()*binScale );
  }
}

class Demo {
  ArrayList< ParticleCloud > myCloud;
  Distribution dist;
  int sampleSize;
  int rate;
  int ctr;
  boolean moving;
  
  Demo( Distribution d, Histogram h , int n ) {
    dist = d;
    myCloud = new ArrayList< ParticleCloud >();
    myCloud.add( new ParticleCloud( h ) );
  
    sampleSize = n;
    moving = false;
    rate = 20;
    ctr = 0;
  }
  
  void add( Histogram h , float s ) {
    ParticleCloud p = new ParticleCloud( h );
    p.setSize( s );
    myCloud.add( p );
  }
  
  void setSampleSize(int n) {
    if(n>0) {
      sampleSize = n;
      for ( ParticleCloud p : myCloud ) {
        p.clear();
      }
    }
  }
  
  void setDist(Distribution d) {
      dist = d;
      for ( ParticleCloud p : myCloud ) {
        p.clear();
      }
  }
  
  void setRate(int n) {
    if ( n > 0 ) {
      rate = n;
    }
  }
  
  void update() {
    if( moving ) {
      moving = false;
      for ( ParticleCloud p : myCloud ) {
        p.update();
        if ( ! p.isDone() ) {
          moving = true;
        }
      }
    }
    
    if ( ctr==0 ) {
      Sample s = new Sample( sampleSize , dist );
      for ( ParticleCloud p : myCloud ) {
        p.calc(s);
      }
      moving = true;

    }
    
    ctr++;
    
    if( ctr>rate ) {
      ctr = 0;
    }
  }
  
  void draw() {
    float a , b;
    a = 100;
    for ( ParticleCloud p : myCloud ) {
        if ( p.destHist.isVisible() ) {
          b = p.destHist.findScale();
          if ( b < a ) {
            a = b;
          }
        }
    }
    
    
    
    for ( ParticleCloud p : myCloud ) {
        p.destHist.setScale( a );
        p.destHist.draw();
    }
    
    for ( ParticleCloud p : myCloud ) {
        p.draw();
    }
  }
}

  
  
    
class Distribution {
  float mu;
  float sigma;
  
  Distribution() {
    mu = 0;
    sigma = 0;
  }
  
  float get() {
    return 0;
  }
}

class Histogram {
  float xPos;
  float yPos;
  float histWidth;
  float histHeight;
  float histMin;
  float histMax;
  ArrayList<Bin> bins;
  color myColor;
  boolean visible;
  
  Histogram( float x , float y , float w , float h ) {
    xPos = x;
    yPos = y;
    histWidth = w;
    histHeight = h;
    bins = new ArrayList<Bin>();
    myColor = color(150);
    visible = true;
  }
  
  void setColor(color c) {
    myColor = c;
    for(Bin b: bins) {
      b.setColor(c);
    }
  }
  
  void createBins( float l , float r , int n ) {
    histMin = l;
    histMax = r;
    Bin b;
    float binWidth = (r - l) / n;
    float rectWidth = histWidth / n;
    for( int i = 0 ; i<n ; i++) {
      b =new Bin( xPos + i * rectWidth , yPos , rectWidth , histHeight , l + i*binWidth , l+(i+1) * binWidth );
      b.setColor(myColor);
      bins.add(b);
    }
    bins.get(n-1).setStrictRight();
    
  }
  
  void reZero() {
    for(Bin b: bins) {
      b.reZero();
    }
  }
  
  
  float min() {
    return histMin;
  }
  
  float max() {
    return histMax;
  }
  
  
  float getMax() {
    float m = 0;
    float n;
    float t = float( getTotal() );
    for(Bin b: bins) {
      n = b.get();
      if(n > m) {
      m = n;
      }
    }
    return m;
  }
  
  int getTotal() {
    int n = 0;
    for(Bin b: bins) {
      n = n + b.binCnt;
    }
    return n;
  }  
  
  float findScale() {
    return histHeight / getMax();
  }
  

  void setScale( float s ) {
    for(Bin b: bins) {
       b.setScale(s);
    } 
    
  }

  float getX(float v) {
    return xPos + histWidth * ( v - histMin ) / ( histMax - histMin );
  }
  
  void show() {
    visible = true;
  }
  
  void hide() {
    visible = false;
  }
  
  void flip() {
    if ( visible ) {
      visible = false;
    }
    else {
      visible = true;
    }
  }
  
  boolean isVisible() {
    return visible;
  }  
  
  void draw() {
    if ( visible ) {
      fill(myColor);
      for(Bin b: bins) {
        b.draw();
      }
    }
  }
  
  ArrayList<Float> calc( Sample s ) {
    return s.values;
  }
  
  void add( float v ) {
    for( Bin b: bins ) {
      if ( b.contains( v ) ) {
        b.increment();
      }
    }
  }
}
class Normal extends Distribution {
  
  Normal( float m , float s ) {
    mu = m;
    sigma = s;
  }
  
  Normal() {
    this(0,1);
  }
  
  float get() {
    return randomGaussian()*sigma+mu;
  }
  
  float randomGaussian() {
    float x1, x2, y1, y2;
    x1 = random(1);
    x2 = random(2);
    y1 = sqrt( -2 * (log(x1))) * cos(2  * PI * x2);
    y2 = sqrt( -2 * (log(x1))) * sin(2  * PI * x2);
    return y1;
  }
}

class ParticleCloud {
  ArrayList<Ball> balls;
  float startHeight;
  Histogram destHist;
  boolean moving;
  float ballSize;
  color myColor;
  
  ParticleCloud( Histogram h1 ) {
    startHeight = 0;
    destHist = h1;
    balls = new ArrayList<Ball>();
    ballSize = 10;
    moving = false;
    myColor = destHist.myColor;
  }
  
  void setSize(float v) {
    ballSize = v;
    for(Ball b : balls) {
      b.setSize(v);
    }
  }
    
  void setColor(color c) {
    myColor = c;
    for(Ball b : balls) {
      b.setColor(c);
    }
  }
  
  void update() {
    boolean m = false;
    Ball b;
    for(int j = balls.size() -1; j>=0 ; j--) {
      b = balls.get(j);
      b.update();
      if( b.isDone() ) {
        destHist.add( b.getValue() );
        balls.remove(j);
      }
      else {
        m = true;
      }
      
      
    }
    moving = m;
    
  }
  
  boolean isDone() {
    if( moving ) {
      return false;
    }
    else {
      return true;
    }
  }
  
  void draw() {
    if ( destHist.isVisible() ) {
      
      for ( Ball b: balls ) {
        b.draw();
      }
    }
  }
  
  void clear() {
    moving = false;
    balls = new ArrayList<Ball>();
    destHist.reZero();
  }
  
  void add( float v ) {
    if ( (v > destHist.min()) && (v < destHist.max())) {
      Ball b = new Ball( destHist.getX(v) , startHeight , destHist.yPos , v );
      b.setSize( ballSize );
      b.setColor( myColor );
      balls.add( b );
      moving = true;
    }
  }
  
  void add( Sample s ) {
    for( float v : s.values ) {
      add( v );
    }
  }
  
  void calc( Sample s ) {
    ArrayList< Float > c = destHist.calc( s );
    for( float v : c ) {
      add( v );
    }
  }
}

class Sample {
  ArrayList<Float> values;
  Distribution dist;
  int size;
  
  Sample( int n , Distribution d ) {
    values = new ArrayList<Float>();
    dist = d;
    size = n;
    for(int i = 0 ; i<n ; i++) {
      values.add( d.get() );
    }
  }

  float mean() {
    float sum = 0;
    for( float v : values ) {
      sum = sum + v;
    }
    return sum/float(values.size());
  }
  
  float stDev() {
    float sumSq = 0;
    for( float v : values ) {
      sumSq = sumSq + v * v;
    }
    float m = mean();
    return ( sumSq  - size * m * m ) / ( size - 1 );
  }
  
  float normalize() {
    return ( mean() - dist.mu ) / ( dist.sigma / sqrt( size ) );
  }
  
  float tStat() {
    return ( mean() - dist.mu ) / ( stDev() / sqrt( size ) );
  }
  
}
class Uniform extends Distribution {
  float uniLeft;
  float uniRight;
  
  Uniform( float l , float r ) {
    uniLeft = l;
    uniRight = r;
    mu = ( l + r ) / 2;
    sigma = ( r - l )*( r - l ) / 12;
  }
  
  Uniform() {
    this(0,1);
  }
  
  float get() {
    return random(uniLeft,uniRight);
  }
}
class meanHist extends Histogram {
  meanHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.mean() );
    return c;
  }
}
class normHist extends Histogram {
  normHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.normalize() );
    return c;
  }
}
class tHist extends Histogram {
  tHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.tStat() );
    return c;
  }
}  

