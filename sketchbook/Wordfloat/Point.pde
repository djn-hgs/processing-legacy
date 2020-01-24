class Word {
  PVector pos;
  PVector vel;
  int size;
  int life;
  int fade;
  float wWidth;
  String word;
  int fadeCount;
  
  Word( String w , int s , PVector p , PVector v , int f) {
    pos = p.get();
    vel = v.get();
    word = w;
    life = 255;
    size = s;
    fade = f;
    fadeCount=0;
    int fsize = constrain(size, 0, 50);
    textSize(fsize);
    wWidth = textWidth(w);
  }
  
  int size() {
    return size;
  }
  
  void display() {
    if (life>12) {
      int fsize = constrain(size, 0, 50);
      textSize(fsize);
      fill(200,life);
      text(word, pos.x, pos.y);
    }
  }
  
  void update() {
    pos.add(vel);
  }
  
  void bigger() {
    size++;
    life=255;
  }
  
  void fade() {
    fadeCount++;
    if (fade > 0) {
        if (fadeCount>fade) {
          fadeCount = 0;
          if (life>size) {
            life--;
        }
      }
    }
  }
  
  Boolean isOffScreen() {
    if ((pos.x+wWidth)<0) {
      return true;
    }
    return (pos.x>width);
  }
  
  Boolean isOnScreen() {
     if ((pos.x+wWidth)>width) {
       return false;
    }
    return true;
  }
  
  PVector position() {
    return pos;
  }
  
  float Width() {
    return wWidth;
  }
}
