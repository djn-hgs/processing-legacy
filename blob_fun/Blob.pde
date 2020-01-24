class Blob {
PVector pos;
PVector vel;
PVector acc;
float mass;
float radius;
float G =10000;
boolean isVisible = true;
boolean isFixed = false;
       
    Blob( PVector pos , float mass ) {
    this.pos = pos.copy();
    this.vel = new PVector( 0 , 0 );
    this.acc = new PVector( 0 , 0 );
    this.radius = 6;
    this.mass = mass;
    }
    
    void display() {
     if ( isVisible ) {
      ellipse( pos.x , pos.y , radius , radius );
     }  
  }
    
    void setFixed( boolean f ) {
      isFixed = f;
    }
    
    void setVisible( boolean v ) {
      isVisible = v;
    }
    
      
    void update( float dt ) {
      if ( ! isFixed ) {
      PVector pStep = vel.copy();
      pStep.mult( dt );
      pos.add( pStep );

      PVector vStep = acc.copy();
      vStep.mult( dt );
      vel.add( vStep );
      }
    }
    
    void applyForces() {
      float d = dist( pos.x , pos.y , mouseX , mouseY );
      if ( d > radius ) {
        acc = new PVector( mouseX - pos.x , mouseY - pos.y );
        acc.normalize();
        acc.mult( G / ( d * d ) );
      }
    }
}