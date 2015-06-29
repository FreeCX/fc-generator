#include "font.h"
#include "stack.h"

SDL_Window *window;
SDL_Renderer *render;
SDL_Event event;
short quit_flag = 0;
font_table_t *ft = NULL;
wchar_t example_text[] =
    L"+----------------------+\n"
    L"#    $ 1234567890 $    #\n"
    L"#  this english text!  #\n"
    L"#  это русский текст!  #\n"
    L"+----------------------+";
char * filename;
config_t config;
lang_t lang;

#define swap16(value) \
    ((((unsigned short)((value) & 0x00FF)) << 8) | \
    (((unsigned short)((value) & 0xFF00)) >> 8))

void send_error( int code ) {
    fprintf( stderr, "%s", SDL_GetError() );
    exit( code );
}

void config_loader( void ) {
    size_t readed, offset = 0;
    FILE * f;

    f = fopen( "../config.cfg", "rb" );
    if ( f == NULL ) {
        perror( "fopen" );
        exit( 0 );
    }
    memset( &lang, 0, sizeof( lang_t ) );
    fread( &config, sizeof( config_t ), 1, f );
    filename = (char *) calloc( config.name_size + 1, sizeof( char ) );
    fread( filename, config.name_size, 1, f );
    fprintf( stderr, "filename = %s (%d)\n", filename, config.name_size );
    fprintf( stderr, "WxH = %dx%d", config.img_width, config.img_height );
    offset += 3 * sizeof( u32 ) + config.name_size;
    do {
        fseek( f, offset, SEEK_SET );
        fprintf( stderr, "\n\noffset = %d\n", offset );
        readed = fread( &lang, sizeof( lang_t ), 1, f );
        fprintf( stderr, "readed = %d\n", readed );
        fprintf( stderr, "unicode = %d\n", lang.unicode );
        fprintf( stderr, "abc_size = %d\n", lang.size );
        if ( lang.unicode > 1 ) {
            break;
        }
        size_t read_size = lang.unicode ? 2 : 1;
        fprintf( stderr, "read_size = %d\n", read_size );
        // 5 -- header sizem
        for ( size_t i = lang.size - 5; i; i-- ) {
            wchar_t tmp = 0;
            fread( &tmp, read_size, 1, f );
            if ( lang.unicode ) {
                tmp = swap16( tmp );
            }
            putwc( tmp, stderr ); // ?
            stack_push( tmp );
        }
        offset += lang.size;
    } while ( readed != 0 || !feof( f ) );
    fclose( f );
}

void font_destroy( font_table_t *t ) {
    SDL_DestroyTexture( t->font );
    free( t->table );
    free( t );
}

void font_color( font_table_t *t, Uint32 color ) {
    SDL_SetTextureColorMod( t->font, color >> 16, ( color >> 8 ) & 0xFF, color & 0xFF );
}

void font_draw( SDL_Renderer *r, font_table_t *t, const wchar_t *text, int x, int y ) {
    SDL_Rect wnd = { 0, 0, t->t_width, t->t_height };
    SDL_Rect pos = { 0, 0, t->t_width, t->t_height };
    int dy = 0, i = 0, id = 0, old_x = x;
    wint_t current;

    pos.x = x; pos.y = y;
    while ( ( current = text[i++] ) != '\0' ) {
        switch ( current ) {
            case '\n':
                pos.y += t->t_height;
                pos.x = old_x;
                continue;
            case '\t':
                pos.x += 2 * t->t_width;
                continue;
            case ' ':
                pos.x += t->t_width;
                continue;
            // to upper
            // case 'a'...'z':
            // case L'а'...L'я':
            //     current -= 0x20;
            //     break;
        }
        id = t->table[current] - 1;
        while ( id * t->t_width >= t->f_width ) {
            id -= t->f_width / t->t_width;
            dy++;
        }
        wnd.x = id * t->t_width; wnd.y = dy * t->t_height;
        pos.x += t->t_width;
        SDL_RenderCopy( r, t->font, &wnd, &pos );
        dy = 0;
    }
}

void fill_ft( SDL_Renderer * r, font_table_t ** t ) {
    SDL_Texture * tex = NULL;
    font_table_t * a = NULL;
    a = (font_table_t *) calloc( 1, sizeof( font_table_t ) );
    a->table = (int *) calloc( 0xffff, sizeof( int ) );
    *t = a;
    a->t_width = config.img_width;
    a->t_height = config.img_height;
    tex = IMG_LoadTexture( r, filename );
    free( filename );
    if ( tex == NULL ) {
        fprintf( stderr, "error!" );
        exit( 0 );
    }
    SDL_QueryTexture( tex, NULL, NULL, &( a->f_width ), &( a->f_height ) );
    size_t ssize = stack_get_size();
    fprintf( stderr, "stack_size = %lu\n", ssize );
    for ( size_t i = 0; i < ssize; i++ ) {
        wint_t current = stack_pop();
        a->table[current] = ssize - i;
        fprintf( stderr, "id: %02d [%lc:%d]\n", a->table[current], current, current );
    }
    a->font = tex;
}

void game_info( const char * message ) {
    SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_ERROR, "Error", message, NULL );
}

void game_init( void ) {
    window = SDL_CreateWindow( "Font Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
    if ( window == NULL ) {
        send_error( EXIT_FAILURE );
    }
    render = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
    if ( render == NULL ) {
        send_error( EXIT_FAILURE );
    }
    stack_init();
    config_loader();
    fill_ft( render, &ft );
    stack_clean();
    // set orange color
    font_color( ft, 0xFF7F27 );
}

void game_event( SDL_Event *event ) {
    SDL_PollEvent( event );
    switch ( event->type ) {
        case SDL_QUIT:
            quit_flag = 1;
            break;
        default:
            break;
    }
}

void game_render( void ) {
    // background color
    SDL_SetRenderDrawColor( render, 4, 32, 41, 255 );
    SDL_RenderClear( render );
    font_draw( render, ft, example_text, 52, 96 );
    SDL_RenderPresent( render );
}

void game_destroy( void ) {
    font_destroy( ft );
    SDL_DestroyRenderer( render );
    SDL_DestroyWindow( window );
    SDL_Quit();
}

int main( int argc, char *argv[] ) {
    setlocale( LC_CTYPE, "" );
    game_init();
    while ( quit_flag == 0 ) {
        game_event( &event );
        game_render();
    }
    game_destroy();
    return EXIT_SUCCESS;
}
