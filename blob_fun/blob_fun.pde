import org.processing.wiki.triangulate.*;
 
ArrayList triangles = new ArrayList();
ArrayList points = new ArrayList();

ArrayList< Blob > blobs = new ArrayList(); 

float dt;
float lastTime;

void setup() {
        size( 640 , 480 );
        
        PVector bPos;
        
        int x_l = 40;
        int y_l = 40;
        int x_gap = 40;
        int y_gap = 40;
        int x_width = 10;
        int y_width = 10;
        
        for( int i = 0 ; i < x_width ; i++ ) {
                for( int j = 0 ; j < y_width ; j++ ) {
                  
                      bPos = new PVector( x_l + i * x_gap , y_l + j * y_gap );
                      blobs.add( new Blob( bPos , 2 ) );
                  
                     }
                }
        lastTime = millis();
        }

void draw() {
       background( 0 );
       
       float newTime = millis();
       dt = ( newTime - lastTime ) / 1000;
       lastTime = newTime;
       
       points.clear();
       
       for( Blob b : blobs ) {
          points.add( b.pos );         
       }       
       
       triangles = Triangulate.triangulate( points );

       // draw the mesh of triangles
       stroke( 255 , 0 , 0 , 40 );
       fill( 255, 40 );
  
        beginShape(TRIANGLES);
 
        for (int i = 0; i < triangles.size(); i++) {
          Triangle t = (Triangle)triangles.get(i);
          vertex(t.p1.x, t.p1.y);
          vertex(t.p2.x, t.p2.y);
          vertex(t.p3.x, t.p3.y);
        }
        endShape();
       
       stroke( 0 , 40 );
       fill( 255 , 200 );
       
       for( Blob b : blobs ) {
               b.display();
               b.update( dt );
               b.applyForces();  
             }
       }


                                                                       