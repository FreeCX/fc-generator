pub trait StrSpec {
    fn slice( &self, begin: usize, end: usize ) -> &str;
    fn tokenize( &self, delimeters: & str ) -> Vec< String >;
}

pub trait NumSplitter {
    fn rsplit( &self ) -> Vec<u8>;
    fn split( &self ) -> Vec<u8>;
}

impl StrSpec for str {
    fn slice( &self, begin: usize, end: usize ) -> &str {
        assert!( begin <= end );
        let begin_byte = Some( begin );
        let end_byte = if end <= self.len() { Some( end ) } else { Some( self.len() ) };
        match ( begin_byte, end_byte ) {
            ( None, _ ) => panic!( "slice: `begin` is beyond end of string" ),
            ( _, None ) => panic!( "slice: `end` is beyond end of string" ),
            ( Some( a ), Some( b ) ) => unsafe {
                self.slice_unchecked( a, b )
            }
        }
    }
    fn tokenize( &self, delimeters: & str ) -> Vec< String > {
        let mut token: Vec< String > = Vec::new();
        let mut prev = 0;
        let mut next = 0;
        let mut comma = false;

        for i in self.chars() {
            comma = match i {
                '"' => !comma,
                _   => comma
            };
            if !comma {
                for j in delimeters.chars() {
                    if i == j {
                        if next - prev >= 1 {
                            token.push( self.slice( prev, next ).to_owned() );
                            prev = next + 1;
                            break;
                        }
                        prev = next + 1;
                    }
                }
            }
            next += 1;
        }
        // add last token
        if next - prev >= 1 {
            token.push( self.slice( prev, next ).to_owned() );
        }
        token
    }
}

impl NumSplitter for u32 {
    fn rsplit( &self ) -> Vec<u8> {
        let mut data: Vec<u8> = Vec::new();
        data.push( (   self & 0x000000ff )         as u8 );
        data.push( ( ( self & 0x0000ff00 ) >>  8 ) as u8 );
        data.push( ( ( self & 0x00ff0000 ) >> 16 ) as u8 );
        data.push( ( ( self & 0xff000000 ) >> 24 ) as u8 );
        data
    }
    fn split( &self ) -> Vec<u8> {
        let mut data: Vec<u8> = Vec::new();
        data.push( ( ( self & 0xff000000 ) >> 24 ) as u8 );
        data.push( ( ( self & 0x00ff0000 ) >> 16 ) as u8 );
        data.push( ( ( self & 0x0000ff00 ) >>  8 ) as u8 );
        data.push( (   self & 0x000000ff )         as u8 );
        data
    }
}
