class Path {
  PVector start;
  PVector pos;
  PVector finish;
  PVector vel;
  float len;
  float par;
  float spd;
  Boolean isdone;
  
  Path( PVector s , PVector f , float l , float v ) {
    start = s.get();
    finish = f.get();
    
    len = l;
    par = 0;
    spd = v;
    
    pos = start.get();
    vel = finish.get();
    vel.sub(start);
    vel.mult( v/l );
    
    isdone = false;
  }
  
  void update() {
    if (!isdone) {
      par = par + spd;
      if ( par >= len ) {
        pos = finish.get();
        isdone = true;
      }
      else {
        pos.add( vel );
      }
    }
  }
}
