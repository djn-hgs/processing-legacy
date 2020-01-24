class Path {
  PVector start;
  PVector control;
  PVector finish;
  
  Path( PVector s , PVector c , PVector f ) {
    start = s.get();
    control = c.get();
    finish = f.get();
  }

  PVector position( float t ) {
    PVector a = control.sub(start);
    PVector b = finish.sub(control);
    a.mult(t);
    b.mult(t);
    a.add(start);
    b.add(control);
    b.sub(a);
    b.mult(t);
    a.add(b);
    return a;
  }
}    
