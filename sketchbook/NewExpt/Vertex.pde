class Vertex extends PVector {
   PVector pos;
   new Vertex(PVector p) {
     pos = p.get();
   }
   
   void display() {
     point(pos.x,pos.y);
   }
   
   Vertex get() {
     return new Vertex(pos.x,pos.y);
   }
}
