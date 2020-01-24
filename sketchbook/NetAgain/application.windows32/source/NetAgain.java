import processing.core.*; 
import processing.data.*; 
import processing.event.*; 
import processing.opengl.*; 

import peasy.*; 

import java.util.HashMap; 
import java.util.ArrayList; 
import java.io.File; 
import java.io.BufferedReader; 
import java.io.PrintWriter; 
import java.io.InputStream; 
import java.io.OutputStream; 
import java.io.IOException; 

public class NetAgain extends PApplet {



ArrayList<Polyhedron> myNets;
float t;
PeasyCam cam;

public void setup() {
	frameRate(60);
	size(800,600,P3D);
	cam = new PeasyCam(this,400);
	myNets = new ArrayList<Polyhedron>();

	Cube c = new Cube(50,200,-50,-50);
	c.staircaseNet();
	myNets.add(c);
	
	Pyramid p = new Pyramid(100,-200,-50,-50);
	p.staircaseNet();
	myNets.add(p);
	
	Tetrahedron t = new Tetrahedron(100,0,-50,-50);
	t.staircaseNet();
	myNets.add(t);
	

}
 
public void draw() {
  background(0);
  
  t = mouseX/PApplet.parseFloat(width);
  
  for (Polyhedron c: myNets) {
	c.display(t);
  }
  
}
class Cube extends Polyhedron {
	float s;
	
	Cube(float a,float x,float y,float z) {
	super(x,y,z);
	s = a;
	}
	
	public void staircaseNet() {

    Face[] f = new Face[6];
	
	for (int i=0;i<6;i++) {
		f[i] = new Face(4,s);
	}
	
	myNet.base(f[0]);
	
	// Staircase
	
	myNet.add(f[0],3,f[1],PI/2);
	myNet.add(f[1],1,f[2],PI/2);
	myNet.add(f[2],3,f[3],PI/2);
	myNet.add(f[3],1,f[4],PI/2);
	myNet.add(f[4],3,f[5],PI/2);
	
	}
	
	
	public void longNet() {
	Vertex v1,v2,v3,v4;
	Edge e1,e2,e3,e4;
	Face f1,f2,f3,f4,f5,f6;
	v1 = new Vertex(0,0);
	v2 = new Vertex(0,s);
	v3 = new Vertex(s,s);
	v4 = new Vertex(s,0);
	e1 = new Edge(v1,v2);
	e2 = new Edge(v2,v3);
	e3 = new Edge(v3,v4);
	e4 = new Edge(v4,v1);
    
	f1 = new Face();

	f1.add(e1);
	f1.add(e2);
	f1.add(e3);
	f1.add(e4);

	f2 = new Face(f1);
	f3 = new Face(f1);
	f4 = new Face(f1);
	f5 = new Face(f1);
	f6 = new Face(f1);
  
	myNet.first = f1;
	
	// Long
	myNet.add(f1,e2,f2,PI/2);
	myNet.add(f2,e2,f3,PI/2);
	myNet.add(f3,e2,f4,PI/2);
	myNet.add(f1,e1,f5,PI/2);
	myNet.add(f4,e3,f6,PI/2);

	}

}
class Cuboid extends Polyhedron {
	Vertex v1,v2,v3,v4;
	Edge e1,e2,e3,e4;
	Face f1,f2,f3,f4,f5,f6;
	float s,t,u;
	
	Cuboid(float a,float b,float c,float x,float y,float z) {
	super(x,y,z);
	s = a;
	t = b;
	u = c;	
	}
	
	public void staircaseNet() {
	
	Face[] f = new Face[6];
	
	f[0] = new Face(4,1);
	f[1] = new Face();
	f[2] = new Face();
	f[3] = new Face(f[2]);
	f[4] = new Face(f[1]);
	f[5] = new Face(f[0]);
	
	myNet.base(f[0]);
	
	// Staircase
	
	myNet.add(f[0],3,f[1],PI/2);
	myNet.add(f[1],1,f[2],PI/2);
	myNet.add(f[2],3,f[3],PI/2);
	myNet.add(f[3],1,f[4],PI/2);
	myNet.add(f[4],3,f[5],PI/2);
	
	}
	
	
	public void longNet() {
	v1 = new Vertex(0,0);
	v2 = new Vertex(0,s);
	v3 = new Vertex(s,s);
	v4 = new Vertex(s,0);
	e1 = new Edge(v1,v2);
	e2 = new Edge(v2,v3);
	e3 = new Edge(v3,v4);
	e4 = new Edge(v4,v1);
    
	f1 = new Face();

	f1.add(e1);
	f1.add(e2);
	f1.add(e3);
	f1.add(e4);

	f2 = new Face(f1);
	f3 = new Face(f1);
	f4 = new Face(f1);
	f5 = new Face(f1);
	f6 = new Face(f1);
  
	myNet.first = f1;
	
	// Long
	myNet.add(f1,e2,f2,PI/2);
	myNet.add(f2,e2,f3,PI/2);
	myNet.add(f3,e2,f4,PI/2);
	myNet.add(f1,e1,f5,PI/2);
	myNet.add(f4,e3,f6,PI/2);

	}

}
	//v1 = new Vertex(0,0);
	//v2 = new Vertex(0,s);
	//v3 = new Vertex(s,s);
	//v4 = new Vertex(s,0);
	//e1 = new Edge(v1,v2);
	//e2 = new Edge(v2,v3);
	//e3 = new Edge(v3,v4);
	//e4 = new Edge(v4,v1);
    
	//PVector[] p = new PVector[4];
	//p[0] = new PVector(0,0);
	//p[1] = new PVector(s,0);
	//p[2] = new PVector(s,s);
	//p[3] = new PVector(0,s);
class Edge {
  Vertex start;
  Vertex finish;
  PVector direction;
  
  Edge(Vertex s,Vertex f) {
    start = s;
    finish = f;
	direction = finish.pos.get();
	direction.sub(start.pos.get());
  }
  
  public void display() {
    stroke(255,200);
    line(start.pos.x,start.pos.y,0,finish.pos.x,finish.pos.y,0);
  }  
}
class Face {
	PVector pos;
	ArrayList<Edge> edges;
  
	Face() {
		pos = new PVector(0,0,0);
		edges = new ArrayList<Edge>();
	}
	
	Face(PVector p,ArrayList<Edge> e) {
		pos = p.get();
		edges = e;
	}
  
	Face(Face f) {
		this(f.pos,f.edges);
	}
	
	Face(PVector[] p) {
		pos = new PVector(0,0,0);
		edges = new ArrayList<Edge>();
		int n = p.length;
		Vertex v,vs,vf;
		v = new Vertex(p[0].x,p[0].y);
		vs = v;
		for (int i=1;i<n;i++) {
		vf = new Vertex(p[i].x,p[i].y);
		edges.add(new Edge(vs,vf));
		vs = vf;
		}
		edges.add(new Edge(vs,v));
	}

	Face(int n,float s) {
		pos = new PVector(0,0,0);
		edges = new ArrayList<Edge>();
		Vertex v,vs,vf;
		PVector[] p = new PVector[n];
		float a = TWO_PI/PApplet.parseFloat(n);
		p[0] = new PVector(s,0);
		for (int i=1;i<n;i++) {
			p[i] = new PVector(-s,0);
			p[i].rotate(-a*(i-1));
			p[i].add(p[i-1]);
		}
		v = new Vertex(p[0].x,p[0].y);
		vs = v;
		for (int i=1;i<n;i++) {
		vf = new Vertex(p[i].x,p[i].y);
		edges.add(new Edge(vs,vf));
		vs = vf;
		}
		edges.add(new Edge(vs,v));
	}
	
 
	public void add(Edge e) {
		edges.add(e);
	}
    
	public void display() {
	pushMatrix();
	translate(pos.x,pos.y);
    for (Edge e: edges) {
		e.display();
    }

    fill(100,80);

    beginShape();
    Edge e = edges.get(0);
    vertex(e.start.pos.x,e.start.pos.y);
    for (int i = 0 ; i<edges.size() ; i++) {
		e = edges.get(i);
		vertex(e.finish.pos.x,e.finish.pos.y);
    }
    e = edges.get(0);
    vertex(e.start.pos.x,e.start.pos.y);
    endShape();
	
	popMatrix();
	}
	
	public Edge edge(int n) {
		return edges.get(n);
	}
	
}
class Join {
	float angle;
	Edge edge;
	Face parent;
	Face child;
	
	Join(Face p,Edge e,Face c,float a) {
	parent = p;
	edge = e;
	child = c;
	angle = a;
	}
	
}
class Net {
	PVector pos;
	Face first;
	ArrayList<Join> joins;
	
	Net(PVector p) {
		pos = p.get();
		first = null;
		joins = new ArrayList<Join>();
	}
	
	Net(PVector p,Face f) {
		pos = p.get();
		first = f;
		joins = new ArrayList<Join>();
	}

	public void base(Face f) {
		first = f;
	}
	
	public void add(Face p,Edge e,Face c,float a) {
		joins.add(new Join(p,e,c,a));
	}
	
	public void add(Face p,int n,Face c,float a) {
		joins.add(new Join(p,p.edge(n),c,a));
	}

	public void display(float t) {
		pushMatrix();
		translate(pos.x,pos.y,pos.z);
		this.descendents(first,t);
		popMatrix();
	}
		
	public void descendents(Face f,float t) {
	PVector dir;
	PVector dir2d;
	PVector pivot;
	PVector xdir = new PVector(1,0);
	f.display();
	for (Join j: joins) {
			if (j.parent==f) {
				pivot = j.edge.start.pos;
				dir = j.edge.direction;
				pushMatrix();
				translate(pivot.x,pivot.y,pivot.z);
				rotate(angle(xdir,dir),0,0,1);
				rotateX(t*j.angle);
				descendents(j.child,t);
				popMatrix();
			}
		}
	}
	
	public float angle(PVector v1,PVector v2) {
		float a = atan2(v2.y,v2.x) - atan2(v1.y,v1.x);
		if (a<0) a += TWO_PI;
		return a;
	}
}
class Polyhedron {
	Net myNet;
	PVector pos;
	
	Polyhedron(float x,float y,float z) {
		pos = new PVector(x,y,z);
		myNet = new Net(pos);
	}
		
	public void display(float t) {
		myNet.display(t);

	}
}
class Pyramid extends Polyhedron {
	float s;
	
	Pyramid(float a,float x,float y,float z) {
	super(x,y,z);
	s = a;
	}
	
	public void flowerNet() {

    Face[] f = new Face[5];
	
	f[0]=new Face(4,s);
	
	for (int i=1;i<5;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Flower
	float a;
	a = PI-acos(1/sqrt(3));
	myNet.add(f[0],0,f[1],a);
	myNet.add(f[0],1,f[2],a);
	myNet.add(f[0],2,f[3],a);
	myNet.add(f[0],3,f[4],a);
	
	}
	
	public void staircaseNet() {

    Face[] f = new Face[5];
	
	f[0]=new Face(4,s);
	
	for (int i=1;i<5;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Staircase
	float a;
	a = acos(1/sqrt(3));
	myNet.add(f[0],0,f[1],PI-a);
	myNet.add(f[1],2,f[2],PI-2*a);
	myNet.add(f[2],1,f[3],PI-2*a);
	myNet.add(f[3],1,f[4],PI-2*a);
	
	}
}
class Tetrahedron extends Polyhedron {
	float s;
	
	Tetrahedron(float a,float x,float y,float z) {
	super(x,y,z);
	s = a;
	}
	
	public void flowerNet() {

    Face[] f = new Face[4];
	
	for (int i=0;i<4;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Flower
	float a;
	a = PI-acos(1/3.f);
	myNet.add(f[0],0,f[1],a);
	myNet.add(f[0],1,f[2],a);
	myNet.add(f[0],2,f[3],a);
	
	}
	
	public void staircaseNet() {

    Face[] f = new Face[4];
	
	for (int i=0;i<4;i++) {
		f[i] = new Face(3,s);
	}
	
	myNet.base(f[0]);
	
	// Staircase
	float a;
	a = PI-acos(1/3.f);
	myNet.add(f[0],0,f[1],a);
	myNet.add(f[1],2,f[2],a);
	myNet.add(f[2],1,f[3],a);
	
	}
}
class Vertex {
  PVector pos;
  
  Vertex(float x,float y) {
    pos = new PVector(x,y);
  }
}
  static public void main(String[] passedArgs) {
    String[] appletArgs = new String[] { "--full-screen", "--bgcolor=#666666", "--stop-color=#cccccc", "NetAgain" };
    if (passedArgs != null) {
      PApplet.main(concat(appletArgs, passedArgs));
    } else {
      PApplet.main(appletArgs);
    }
  }
}
