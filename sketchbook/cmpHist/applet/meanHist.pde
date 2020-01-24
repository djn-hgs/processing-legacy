class meanHist extends Histogram {
  meanHist( float x , float y , float w , float h ) {
    super( x , y , w , h );
  }

  ArrayList<Float> calc( Sample s ) {
    ArrayList< Float > c = new ArrayList< Float >();
    c.add( s.mean() );
    return c;
  }
}
