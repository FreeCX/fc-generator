use std::io::prelude::*;
use std::fs::File;
use traits::*;

#[derive(Debug)]
pub struct Lang {
    pub unicode: u8,
    pub size: u32,
    pub abc: String
}

#[derive(PartialEq, Debug)]
pub enum LangParam {
    None,
    Default,
    Unicode
}

#[derive(Debug)]
pub struct ConfigFile {
    pub name_size: u32,
    pub img_width: u32,
    pub img_height: u32,
    pub name: String,
    pub lang: Vec<Lang>
}

impl ConfigFile {
    pub fn new() -> ConfigFile {
        ConfigFile {
            name_size: 0,
            img_width: 0,
            img_height: 0,
            name: String::new(),
            lang: Vec::new()
        }
    }
    pub fn encode( &self, config: &mut File ) {
        use std::slice;
        unsafe {
            config.write_all( slice::from_raw_parts( self.name_size.rsplit().as_ptr(), 4 ) )
                .is_ok();
            config.write_all( slice::from_raw_parts( self.img_width.rsplit().as_ptr(), 4 ) )
                .is_ok();
            config.write_all( slice::from_raw_parts( self.img_height.rsplit().as_ptr(), 4 ) )
                .is_ok();
            config.write_all( self.name.as_bytes() )
                .is_ok();
        }
        for lang in &self.lang {
            unsafe {
                config.write_all( &[lang.unicode] )
                    .is_ok();
                config.write_all( slice::from_raw_parts( lang.size.rsplit().as_ptr(), 4 ) )
                    .is_ok();
                config.write_all( lang.abc.as_bytes() )
                    .is_ok();
            }
        }
    }
}
