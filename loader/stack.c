#include "stack.h"

wchar_t * stack_buffer;
size_t stack_pointer = 0;
size_t stack_size = 80;

void stack_init( void ) {
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
    if ( stack_pointer != 0 ) {
        return stack_buffer[--stack_pointer];
    } else {
        return 0;
    }
}

size_t stack_get_size( void ) {
    return stack_pointer;
}
