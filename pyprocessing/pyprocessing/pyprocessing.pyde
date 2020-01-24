import blob

def setup():
    size( 480 , 300 )
    e = blob( 240 , 150 , 20 )

def draw():
    # background( 255 , 0.8 )
    # if mousePressed:
    #     fill( 0 )
    # else:
    #     fill( 255 )
    # ellipse( mouseX , mouseY , 50 , 50 )
    e.draw()
    e.set_pos( mouseX , mouseY )
    