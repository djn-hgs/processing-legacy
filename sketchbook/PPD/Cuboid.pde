class Cuboid extends Polyhedron {
  
  void build( PVector i, PVector j , PVector k ) {
    PVector o = new PVector( 0 , 0 , 0 );

    PVector P000,P100,P010,P001,P101,P011,P110,P111;

    Face fx = new Face();
    Face fy = new Face();
    Face fz = new Face();
    Face fxx = new Face();
    Face fyy = new Face();
    Face fzz = new Face();
    
    P000=o.get();
    P100=i.get();
    P010=j.get();
    P001=k.get();
    P101=P100.get();
    P101.add(k);
    P011=P010.get();
    P011.add(k);
    P110=P100.get();
    P110.add(j);
    P111=P110.get();
    P111.add(k);
    
    fx.vertex(o);
    fx.vertex(P010);
    fx.vertex(P011);
    fx.vertex(P001);
    
    faces.add(fx);
    
    fxx.clone( fx );
    fxx.move( i );
    faces.add( fxx );
    
    fy.vertex( o );
    fy.vertex(P100);
    fy.vertex(P101);
    fy.vertex(P001);
    
    faces.add( fy );
    
    fyy.clone( fy );
    fyy.move( j );
    faces.add( fyy );
    
    fz.vertex(o);
    fz.vertex(P100);
    fz.vertex(P110);
    fz.vertex(P010);
    
    faces.add(fz);
    
    fzz.clone( fz );
    fzz.move(k);
    faces.add(fzz);
    
    
  }
}
