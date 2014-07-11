#ifndef H_NEOPIXEL
#define H_NEOPIXEL

#include <avr/io.h>
#include <stdio.h>

#ifdef __cplusplus
 extern "C" {
#endif

typedef volatile uint8_t* port8_addr_t;
typedef uint8_t port_pin_t;

/* Hardware NeoPixel LED Functions */
void send_pixel_color(port8_addr_t port_addr, port_pin_t pin, uint8_t color);

void show(void);

#ifdef __cplusplus
}
#endif 

#endif
