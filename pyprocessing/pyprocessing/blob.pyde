class blob:
    def __init__( self , x , y , r ):
        self.x = x
        self.y = y
        self.r = r
   
    def draw( self ):
        ellipse( self.x , self.y , self.r , self.r )

    def set_pos( self , x , y ):
        self.x = x
        self.y = y
