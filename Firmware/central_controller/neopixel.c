#include "neopixel.h"

/* The code below was taken from 
 * http://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 * and adopted to the needs of the program and further simplified
 */


#define PIXELS_NUM  27  // Number of pixels in the string

// These are the timing constraints taken from the WS2812B datasheet 
#define T1H  800    // Width of a 1 bit in ns
#define T1L  450    // Width of a 1 bit in ns

#define T0H  400    // Width of a 0 bit in ns
#define T0L  850    // Width of a 0 bit in ns

#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays

#define NS_PER_SEC (1000000000L)          // TODO Note that this has to be SIGNED since we want to be able to check for negative values of derivatives

#define CYCLES_PER_SEC   (F_CPU)

#define NS_PER_CYCLE     (NS_PER_SEC / CYCLES_PER_SEC)

#define NS_TO_CYCLES(n)  ((n) / NS_PER_CYCLE)

#define DELAY_CYCLES(n)  (((n) > 0) ? __builtin_avr_delay_cycles(n) : __builtin_avr_delay_cycles(0))  // Make sure we never have a delay less than zero


// Actually send a bit to the string. We turn off optimizations to make sure the compile does
// not reorder things and make it so the delay happens in the wrong place.
static void send_bit(port8_addr_t port_addr, port_pin_t pin, uint8_t bit_val) __attribute__ ((optimize(0)));
{
    if (bit_val)
    {
        set_bit(*port_addr, pin);

        DELAY_CYCLES(NS_TO_CYCLES(T1H) - 2);       // 1-bit width less  overhead  for the actual bit setting
        // Note that this delay could be longer and everything would still work
        unset_bit(*port_addr, pin);

        DELAY_CYCLES(NS_TO_CYCLES(T1L) - 2);       // TODO 1-bit gap less the overhead
    }
    else
    {
        set_bit(*port_addr, pin);

        DELAY_CYCLES(NS_TO_CYCLES(T0H) - 2);       // 0-bit width less overhead 
        // **************************************************************************
        // This line is really the only tight goldilocks timing in the whole program!
        // **************************************************************************
        unset_bit(*port_addr, pin);

        DELAY_CYCLES(NS_TO_CYCLES(T0L) - 2);       // TODO 0-bit gap less overhead
    }
}  
  
static void send_byte_fast(uint8_t byte)
{
    // Neopixel wants bit in highest-to-lowest order
    send_bit(byte & 0x80);
    send_bit(byte & 0x40);
    send_bit(byte & 0x20);
    send_bit(byte & 0x10);
    send_bit(byte & 0x08);
    send_bit(byte & 0x04);
    send_bit(byte & 0x02);
    send_bit(byte & 0x01);
} 


void send_pixel(uint8_t r, uint8_t g, uint8_t b)
{
    // Note: Follow the order of GRB to sent data and the high bit sent at first. 
    send_byte_fast(g);
    send_byte_fast(r);
    send_byte_fast(b);
}


// Just wait long enough without sending any bots to cause the pixels to latch and display the last sent frame
void show()
{
    DELAY_CYCLES(NS_TO_CYCLES(RES));               
}
