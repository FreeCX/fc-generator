extern crate font_generator;
use std::io::prelude::*;
use std::fs::File;
use std::path::Path;
use std::io;
use font_generator::traits::*;
use font_generator::structs::*;

fn read_line( text: &str ) -> String {
    let mut buffer = String::new();
    print!( "{}", text );
    io::stdout().flush()
        .ok()
        .expect( "Can't flush to stdout!");
    io::stdin().read_line( &mut buffer )
        .ok()
        .expect( "Can't read to string!" );
    buffer.trim_right().to_string()
}

fn main() {
    let config_file = read_line( "enter config file: " );
    let output_file = read_line( "enter output file: " );
    let output_path = Path::new( &config_file );
    let path = output_path.parent().unwrap();
    let mut file = match File::open( &output_path ) {
        Ok( file )      => file,
        Err( io_error ) => panic!( io_error )
    };
    let mut buffer = String::new();
    file.read_to_string( &mut buffer )
        .ok()
        .expect( "Can't read to string!" );
    let mut config = buffer.tokenize( " [,=]\n" );
    config.reverse();
    let mut config_raw = ConfigFile::new();
    let mut flag = LangParam::None;
    while let Some( current ) = config.pop() {
        match current.trim() {
            "filename"     => {
                let next = config.pop().unwrap();
                config_raw.name = next.trim().slice( 1, next.len() - 1 ).to_string().clone();
                config_raw.name.shrink_to_fit();
                config_raw.name_size = ( config_raw.name.capacity() ) as u32;
            },
            "size"         => {
                let width: u32 = config.pop().unwrap().parse()
                    .ok()
                    .expect( "Can't read a value!" );
                let height: u32 = config.pop().unwrap().parse()
                    .ok()
                    .expect( "Can't read a value!" );
                config_raw.img_width = width;
                config_raw.img_height = height;
            },
            "lang_default" => {
                flag = LangParam::Default;
                continue;
            },
            "lang_unicode" => {
                flag = LangParam::Unicode;
                continue;
            }
            _              => {}
        }
        if flag == LangParam::Default || flag == LangParam::Unicode {
            let tmp = if current.starts_with( "\"" ) {
                current.clone()
            } else {
                config.pop().unwrap()
            };
            let filename = format!( "{}/{}", path.to_str().unwrap(),
                tmp.slice( 1, tmp.len() - 1 ) );
            let mut file = match File::open( &Path::new( &filename ) ) {
                Ok( file )      => file,
                Err( io_error ) => panic!( io_error )
            };
            let mut buffer = String::new();
            file.read_to_string( &mut buffer )
                .ok()
                .expect( "File read failure!" );
            let mut buffer = buffer.trim().to_string();
            buffer.shrink_to_fit();
            let unicode = if flag == LangParam::Unicode { 1_u8 } else { 0_u8 };
            config_raw.lang.push( Lang {
                unicode: unicode,
                size: if unicode == 1_u8 {
                    ( buffer.len() / 2 + 5 ) as u32
                } else {
                    ( buffer.len() + 5 ) as u32
                }, // bool + u32 + String.len()
                abc: buffer.clone()
            } );
        }
    }
    println!( "{:?}", config_raw );
    let mut config = match File::create( &output_file ) {
        Ok( file )      => file,
        Err( io_error ) => panic!( "file error: {}", io_error ),
    };
    config_raw.encode( &mut config );
}
