class Link {
  float nat_len; // natural length
  float lambda; // modulus of elasticity
  Blob b1, b2; // blobs at end of spring

  Link(float l, float a, Blob blob1, Blob blob2) {
    b1=blob1;
    b2=blob2;
    nat_len =l;
    lambda= a;
  }

  float length() {
    PVector d = PVector.sub(b1.pos, b2.pos);
    return(sqrt(d.x*d.x+d.y*d.y+d.z*d.z));
  }

  float extension() {
    return(length()-nat_len);
  }

  float force() {
    return(lambda*extension()/nat_len);
  }

  void draw() {
    stroke(255);
    line(b1.pos.x, b1.pos.y, b2.pos.x, b2.pos.y);
  }
}

