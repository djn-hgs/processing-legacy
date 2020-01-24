float rMid;
float iMid;
float rSize;
float iSize;
int max;
boolean wait;

void setup() {
  size(640,480);
  rMid=.24;
  iMid=0;
  rSize=0.2;
  max=250;
  iSize=rSize*height/width;
  render();
}


float rLeft() {
  return rMid-(rSize/2);
}
float rRight() {
  return rMid+(rSize/2);
}

float iLo() {
  return iMid-(iSize/2);
}

float iHi() {
  return iMid+(iSize/2);
}

void render() {
  int i;
  int j;
  float cR;
  float cI;
  float x;
  float y;
  float xN;
  float yN;
  int esc;
  int n;
  
  background(0);
   for (i=0;i<width; i++) {
     for (j=0; j<height; j++) {
       cR=rLeft()+i*rSize/width;
       cI=iLo()+j*iSize/height;
       x=0;
       y=0;
       esc=0;
       for (n=0;n<max;n++) {
         xN=x*x-y*y+cR;
         yN=2*x*y+cI;
         if ((xN*xN+yN*yN)>2) {
           esc =1;
           break;
         }
         x=xN;
         y=yN;
       }
       if (esc==1) {
         fill(255);
         rect(i,j,1,1);
       }
     }
  }
}

void mouseClicked() {
  iSize=iSize/2;
  rSize=rSize/2;
  wait=false;
}


