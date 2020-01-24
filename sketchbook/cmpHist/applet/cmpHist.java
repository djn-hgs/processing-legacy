import processing.core.*; 
import processing.xml.*; 

import java.applet.*; 
import java.awt.Dimension; 
import java.awt.Frame; 
import java.awt.event.MouseEvent; 
import java.awt.event.KeyEvent; 
import java.awt.event.FocusEvent; 
import java.awt.Image; 
import java.io.*; 
import java.net.*; 
import java.text.*; 
import java.util.*; 
import java.util.zip.*; 
import java.util.regex.*; 

public class cmpHist extends PApplet {


Histogram h1;
normHist h2;
tHist  h3;
meanHist h4;
Distribution u;
Distribution n;
Demo myDemo;
int blue;
int red;
int green;
int purple;
boolean uniform;

public void setup() {
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

public void draw() {
  background(140);
  myDemo.update();
  myDemo.draw();
}

public void keyPressed() {
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
  int myColor;
  boolean moving;
  
  Ball( float x, float y ,  float d , float v ) {
    xPos = x;
    yPos = y;
    xDest = xPos;
    yDest = d;
    xVel = 0;
    yVel = 0;
    xAcc = 0;
    yAcc = 0.1f;
    size = 10;
    value = v;
    myColor = color(150);
    moving = true;
  }
  
  public void setColor(int c) {
    myColor = c;
  }
  
  public void setSize(float v) {
   size = v;
  } 
  
  public float getValue() {
    return value;
  }
  
  public void update() {
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
  
  public boolean isDone() {
    if(moving) {
      return false;
    }
    else {
      return true;
    }
  }
    
  
  public void draw() {
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
  int myColor;
  
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
  
  public void setStrictLeft() {
    strictLeft = true;
  }
  
  public void setStrictRight() {
    strictRight = true;
  }
  
  public void setScale( float s ) {
    binScale = s;
  }
  
  public void reZero() {
    binCnt = 0;
  }
  
  public float midX() {
    return xPos+binWidth/2;
  }
  
  public boolean contains( float v ) {
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
  
  public void increment() {
    binCnt++;
  }
  
  public float get() {
    return binCnt/(binRight-binLeft);
  }
  
  public void setColor(int c) {
    myColor = c;
  }
  
  public void draw() {
    
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
  
  public void add( Histogram h , float s ) {
    ParticleCloud p = new ParticleCloud( h );
    p.setSize( s );
    myCloud.add( p );
  }
  
  public void setSampleSize(int n) {
    if(n>0) {
      sampleSize = n;
      for ( ParticleCloud p : myCloud ) {
        p.clear();
      }
    }
  }
  
  public void setDist(Distribution d) {
      dist = d;
      for ( ParticleCloud p : myCloud ) {
        p.clear();
      }
  }
  
  public void setRate(int n) {
    if ( n > 0 ) {
      rate = n;
    }
  }
  
  public void update() {
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
  
  public void draw() {
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
  
  public float get() {
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
  int myColor;
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
  
  public void setColor(int c) {
    myColor = c;
    for(Bin b: bins) {
      b.setColor(c);
    }
  }
  
  public void createBins( float l , float r , int n ) {
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
  
  public void reZero() {
    for(Bin b: bins) {
      b.reZero();
    }
  }
  
  
  public float min() {
    return histMin;
  }
  
  public float max() {
    return histMax;
  }
  
  
  public float getMax() {
    float m = 0;
    float n;
    float t = PApplet.parseFloat( getTotal() );
    for(Bin b: bins) {
      n = b.get();
      if(n > m) {
      m = n;
      }
    }
    return m;
  }
  
  public int getTotal() {
    int n = 0;
    for(Bin b: bins) {
      n = n + b.binCnt;
    }
    return n;
  }  
  
  public float findScale() {
    return histHeight / getMax();
  }
  

  public void setScale( float s ) {
    for(Bin b: bins) {
       b.setScale(s);
    } 
    
  }

  public float getX(float v) {
    return xPos + histWidth * ( v - histMin ) / ( histMax - histMin );
  }
  
  public void show() {
    visible = true;
  }
  
  public void hide() {
    visible = false;
  }
  
  public void flip() {
    if ( visible ) {
      visible = false;
    }
    else {
      visible = true;
    }
  }
  
  public boolean isVisible() {
    return visible;
  }  
  
  public void draw() {
    if ( visible ) {
      fill(myColor);
      for(Bin b: bins) {
        b.draw();
      }
    }
  }
  
  public ArrayList<Float> calc( Sample s ) {
    return s.values;
  }
  
  public void add( float v ) {
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
  
  public float get() {
    return randomGaussian()*sigma+mu;
  }
  
  public float randomGaussian() {
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
  int myColor;
  
  ParticleCloud( Histogram h1 ) {
    startHeight = 0;
    destHist = h1;
    balls = new ArrayList<Ball>();
    ballSize = 10;
    moving = false;
    myColor = destHist.myColor;
  }
  
  public void setSize(float v) {
    ballSize = v;
    for(Ball b : balls) {
      b.setSize(v);
    }
  }
    
  public void setColor(int c) {
    myColor = c;
    for(Ball b : balls) {
      b.setColor(c);
    }
  }
  
  public void update() {
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
  
  public boolean isDone() {
    if( moving ) {
      return false;
    }
    else {
      return true;
    }
  }
  
  public void draw() {
    if ( destHist.isVisible() ) {
      
      for ( Ball b: balls ) {
        b.draw();
      }
    }
  }
  
  public void clear() {
    moving = false;
    balls = new ArrayList<Ball>();
    destHist.reZero();
  }
  
  public void add( float v ) {
    if ( (v > destHist.min()) && (v < destHist.max())) {
      Ball b = new Ball( destHist.getX(v) , startHeight , destHist.yPos , v );
      b.setSize( ballSize );
      b.setColor( myColor );
      balls.add( b );
      moving = true;
    }
  }
  
  public void add( Sample s ) {
    for( float v : s.values ) {
      add( v );
    }
  }
  
  public void calc( Sample s ) {
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

  public float mean() {
    float sum = 0;
    for( float v : values ) {
      sum = sum + v;
    }
    return sum/PApplet.parseFloat(values.size());
  }
  
  public float stDev() {
    float sumSq = 0;
    for( float v : values ) {
      sumSq = sumSq + v * v;
    }
    float m = mean();
    return ( sumSq  - size * m * m ) / ( size - 1 );
  }
  
  public float normalize() {
    return ( mean() - dist.mu ) / ( dist.sigma / sqrt( size ) );
  }
  
  public float tStat() {
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
  
  public float get() {
    return random(uniLeft,uniRight);
  }
}
class meanHist extends Histogram {
  meanHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  public ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.mean() );
    return c;
  }
}
class normHist extends Histogram {
  normHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  public ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.normalize() );
    return c;
  }
}
class tHist extends Histogram {
  tHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  public ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.tStat() );
    return c;
  }
}  
  static public void main(String args[]) {
    PApplet.main(new String[] { "--bgcolor=#F0F0F0", "cmpHist" });
  }
}
