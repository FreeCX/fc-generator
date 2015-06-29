#ifndef __STACK_H__
#define __STACK_H__

#include <stdio.h>
#include <stdlib.h>

void stack_init( void );
void stack_clean( void );
void stack_push( wchar_t item );
wchar_t stack_pop( void );
size_t stack_get_size( void );

#endif
