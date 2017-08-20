#ifndef H_NEOPIXEL
#define H_NEOPIXEL

#include <stdio.h>
#include "hardware.h"

// Color names and numbers
#define BLUE        0
#define LIGHT_BLUE  1
#define GREEN       2
#define YELLOW      3
#define ORANGE      4
#define RED         5
#define PINK        6
#define PURPLE      7

#ifdef __cplusplus
 extern "C" {
#endif

/* Hardware NeoPixel LED Functions */
void light_color_buff(uint8_t pin_mask, uint8_t* color_buff_ptr, uint8_t color_buff_len)
void light_side_color(uint8_t side_num, uint8_t *colors);

#ifdef DEBUG_COLOR_ADJUST
void debug_color_adjust(uint8_t side_num, uint8_t indicators);
#endif

#ifdef __cplusplus
}
#endif 

#endif
