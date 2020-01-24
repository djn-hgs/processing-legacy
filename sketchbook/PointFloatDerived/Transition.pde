class Transition {
  ArrayList<Path> paths;

  Transition(PointCloud p1,PointCloud p2) {
    paths = new ArrayList<Path>();
      for (int i=0; i<p1.size() ; i++) {
      paths.add(new Path(p1.get(i),p2.get(i)));
    }
  }
  
  void snapshot(float t) {
     for (Path p: paths) {
       p.snapshot(t);
   }
  }
 
  void display() {
     for (Path p: paths) {
       p.display();
   }
  }

 
  void update() {
  }
  
}
