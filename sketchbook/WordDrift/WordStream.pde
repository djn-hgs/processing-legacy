class WordStream {
  ArrayList<Word> words;
  
  WordStream() {
    words = new ArrayList<Word>();
  }
  
  void display() {
   for (Word w: words) {
     w.display();
   }
  }
  
  void addWord(String w,int s,PVector p,PVector v, int f) {
    Word word = new Word ( w , s , p , v, f); 
    int pos = words.size();
    words.add( word );
  }
  
  void update() {
    for (int i=words.size()-1 ; i>=0 ; i-- ) {
      Word w = words.get(i);
      w.update();
      w.fade();
      if (w.isOffScreen()) {
        words.remove(i);
      }
    }
  }
  
  Boolean isOnScreen() {
    int pos = words.size();
    if (pos==0) {
      return true;
    }
    Word word = words.get(pos-1);
    return word.isOnScreen();
  }
  
  Word get(int pos) {
    return words.get(pos);
  }
}
