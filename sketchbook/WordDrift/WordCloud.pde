class WordCloud {
  ArrayList<Word> words;
  IntDict lookup;
  
  WordCloud() {
    words = new ArrayList<Word>();
    lookup = new IntDict();
  }
  
  void display() {
   for (Word w: words) {
     w.display();
   }
  }
  
  void addWord(String w,int s,PVector p,PVector v,int f) {
    Word word = new Word ( w , s , p , v, f); 
    int pos = words.size();
    words.add( word );
    lookup.set( w , pos );
  }
  
  void biggerWord(String w) {
    int pos = lookup.get( w );
    Word word = words.get( pos );
    word.bigger();
  }
  
  Boolean hasWord(String w) {
    return lookup.hasKey(w);
  }
  
  int wordSize(String w) {
    if (hasWord(w)) {
      int pos = lookup.get( w );
      Word word = words.get( pos );
      return word.size();
    }
    return 12;
  }
  
  void update() {
    for (int i=words.size()-1 ; i>=0 ; i-- ) {
      Word w = words.get(i);
      w.update();
      w.fade();
    }
  }
  
}

