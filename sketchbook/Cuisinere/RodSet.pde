class RodSet {
  ArrayList<Rod> rods;

  RodSet() {
     rods = new ArrayList<Rod>();
  }
    
  void display() {
    for ( Rod r: rods ) {
      r.display();
    }
  }

  void update() {
    for ( Rod r: rods ) {
      r.update();
    }
  }
  
  void add(Rod r) {
    rods.add( r );
  }
  
  Rod closestRod( float x , float y ) {
    Rod bestRod = null;
    Boolean foundBest = false;
    float distSq = 0;
    for ( Rod r: rods ) {
      float rdistSq = r.dist( x , y );
      if (( rdistSq < distSq ) || (!foundBest)) {
        distSq = rdistSq;
        foundBest = true;
        bestRod = r;
      }
    }
  return bestRod;    
      
  }
  
  Rod clicked( float x , float y ) {
    Rod clickedRod = null;
    Boolean foundClicked = false;
    for ( Rod r: rods ) {
      if ( r.clicked( x , y ) ) {
        clickedRod = r;
        foundClicked = true;
      }
    }
  return clickedRod;
  }
  
}
