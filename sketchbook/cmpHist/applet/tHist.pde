class tHist extends Histogram {
  tHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.tStat() );
    return c;
  }
}  
