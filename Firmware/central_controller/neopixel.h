#ifndef H_NEOPIXEL
#define H_NEOPIXEL

#include <avr/io.h>
#include "hardware.h"

#ifdef __cplusplus
 extern "C" {
#endif

/* Hardware NeoPixel LED Functions */
void light_side_color(uint8_t side_num, uint8_t *colors);

void clear_side_color(uint8_t side_num);

#ifdef __cplusplus
}
#endif 

#endif
