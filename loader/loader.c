#include <stdio.h>
#include <stdlib.h>
#include <wchar.h>
#include <string.h>
#include <locale.h>

typedef unsigned int u32;
typedef unsigned char u8;

#pragma pack(push, 1)
struct Config {
    u32 name_size;
    u32 img_width;
    u32 img_height;
};
#pragma pack(pop)
typedef struct Config config_t;

#pragma pack(push, 1)
struct Lang {
    u8 unicode;
    u32 size;
};
#pragma pack(pop)
typedef struct Lang lang_t;

wchar_t * stack_buffer;
size_t stack_pointer = 0;
size_t stack_size = 100;

void stack_init() {
    stack_buffer = (wchar_t *) calloc( stack_size, sizeof( wchar_t ) );
}

void stack_clean( void ) {
    free( stack_buffer );
}

void stack_push( wchar_t item ) {
    if ( stack_size == stack_pointer ) {
        wchar_t * tmp = (wchar_t *) realloc( stack_buffer, stack_size * 1.2 * sizeof( wchar_t ) );
        stack_buffer = tmp;
        stack_size *= 1.2;
    }
    stack_buffer[stack_pointer] = item;
    stack_pointer++;
}

wchar_t stack_pop( void ) {
    if ( stack_pointer > 0 ) {
        return stack_buffer[stack_pointer];
    } else {
        return 0;
    }
}

#define swap16(value) \
    ((((unsigned short)((value) & 0x00FF)) << 8) | \
    (((unsigned short)((value) & 0xFF00)) >> 8))

int main( void ) {
    config_t config;
    char * filename;
    size_t readed, offset = 0;
    lang_t lang;
    FILE * f;

    setlocale( LC_CTYPE, "" );
    f = fopen( "../config.cfg", "rb" );
    if ( f == NULL ) {
        perror( "fopen" );
        exit( 0 );
    }
    memset( &lang, 0, sizeof( lang_t ) );
    stack_init();
    fread( &config, sizeof( config_t ), 1, f );
    filename = (char *) calloc( config.name_size + 1, sizeof( char ) );
    fread( filename, config.name_size, 1, f );
    printf( "filename = %s (%d)\n", filename, config.name_size );
    printf( "WxH = %dx%d", config.img_width, config.img_height );
    offset += 3 * sizeof( u32 ) + config.name_size;
    do {
        fseek( f, offset, SEEK_SET );
        printf( "\n\noffset = %d\n", offset );
        readed = fread( &lang, sizeof( lang_t ), 1, f );
        printf( "readed = %d\n", readed );
        printf( "unicode = %d\n", lang.unicode );
        printf( "abc_size = %d\n", lang.size );
        if ( lang.unicode > 1 ) {
            break;
        }
        size_t read_size = lang.unicode ? 2 : 1;
        printf( "read_size = %d\n", read_size );
        // 5 -- header sizem
        for ( size_t i = lang.size - 5; i; i-- ) {
            wchar_t tmp = 0;
            fread( &tmp, read_size, 1, f );
            if ( lang.unicode ) {
                tmp = swap16( tmp );
            }
            putwc( tmp, stdout ); // ?
            stack_push( tmp );
        }
        offset += lang.size;
    } while ( readed != 0 || !feof( f ) );
    fclose( f );
    stack_clean();
    return 0;
}
