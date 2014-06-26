#ifndef H_COMMON
#define H_COMMON

#include <avr/io.h>

/* Macros */
#define set_bit(REG, BIT) ((REG) |= _BV(BIT))
#define unset_bit(REG, BIT) ((REG) &= ~_BV(BIT))

#endif
