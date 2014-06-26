#ifndef H_UTILS
#define H_UTILS

#include <stdio.h>


#ifdef __cplusplus
 extern "C" {
#endif 

void unpack_colors(uint8_t *colors_packed, uint8_t *colors);

void pack_colors(uint8_t *colors, uint8_t *colors_packed);


#ifdef __cplusplus
}
#endif 
#endif
