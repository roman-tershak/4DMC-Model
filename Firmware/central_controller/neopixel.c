#include "neopixel.h"
#include "common.h"
#include <avr/builtins.h>

/* The code below was taken from 
 * http://wp.josh.com/2014/05/13/ws2812-neopixels-are-not-so-finicky-once-you-get-to-know-them/
 * and adopted to the needs of the program and further simplified
 */

// These are the timing constraints taken from the WS2812B datasheet 
#define T1H  800    // Width of a 1 bit in ns
#define T1L  450    // Width of a 1 bit in ns

#define T0H  400    // Width of a 0 bit in ns
#define T0L  850    // Width of a 0 bit in ns

#define RES 6000    // Width of the low gap between bits to cause a frame to latch

// Here are some convience defines for using nanoseconds specs to generate actual CPU delays
#define NS_PER_CYCLE     (1000000000L / F_CPU)
#define NS_TO_CYCLES(n)  ((n) / NS_PER_CYCLE)
#define DELAY_CYCLES(n)  (((n) > 0) ? __builtin_avr_delay_cycles(n) : __builtin_avr_delay_cycles(0))  // Make sure we never have a delay less than zero

static const uint8_t COLOR_MATRIX[/*NUM_OF_COLORS*/16][3] = 
{
    {30,  0,  0},   // red
    { 0, 25,  0},   // green
    { 0,  0, 25},   // blue
    {50, 50,  0},   // yellow
    //{ 0, 20, 10},   // light blue
    {0, 15, 5},
    {30,  0, 25},   // pink
    {50,  7,  2},   // orange ...
    {50, 50, 50},   // white
    {10,  0,  0},   // semi-red
    { 0, 20,  0},   // semi-green
    { 0,  0,  8},   // semi-blue
    {20, 20,  0},   // semi-yellow
    { 0,  7,  4},   // semi-light blue
    {10,  0,  8},   // semi-pink
    {20,  2,  1},   // semi-orange ...
    {20, 20, 20}    // semi-white
};


// Actually send a bit to the string. We turn off optimizations to make sure the compile does
// not reorder things and make it so the delay happens in the wrong place.
static void send_byte(port_pin_t pin, uint8_t byte_val) __attribute__ ((optimize(0)));

static void send_byte(port_pin_t pin, uint8_t byte_val)
{
    uint8_t i;

    // Neopixel wants bit in highest-to-lowest order
    for (i = 0x80; i > 0; i >>= 1)
    {
        if (byte_val & i)
        {
            set_bit(SC_PORT, pin);
            DELAY_CYCLES(NS_TO_CYCLES(T1H) - 2);       // 1-bit width less  overhead  for the actual bit setting
            // Note that this delay could be longer and everything would still work
            unset_bit(SC_PORT, pin);
            DELAY_CYCLES(NS_TO_CYCLES(T1L) - 4);       // TODO 1-bit gap less the overhead
        }
        else
        {
            set_bit(SC_PORT, pin);
            DELAY_CYCLES(NS_TO_CYCLES(T0H) - 2);       // 0-bit width less overhead 
            // **************************************************************************
            // This line is really the only tight goldilocks timing in the whole program!
            // **************************************************************************
            unset_bit(SC_PORT, pin);
            DELAY_CYCLES(NS_TO_CYCLES(T0L) - 4);       // TODO 0-bit gap less overhead
        }
    }
}

// Unset pin (just in case) and wait long enough without sending any bots 
// to cause the pixels to latch and display the last sent frame
static void show(port_pin_t pin)
{
    unset_bit(SC_PORT, pin);
    DELAY_CYCLES(NS_TO_CYCLES(RES));               
}

void light_side_color(uint8_t side_num, uint8_t* colors)
{
    uint8_t i;
    uint8_t *rgb_color;
    port_pin_t pin = get_side_led_pin(side_num);

    DISABLE_GLOBAL_INTERRUPTS();

    for (i = 0; i < SIDE_LED_COUNT; i++)
    {
        rgb_color = COLOR_MATRIX[colors[i]];
        // Note: Follow the order of GRB to sent data and the high bit sent at first. 
        send_byte(pin, rgb_color[1]);
        send_byte(pin, rgb_color[0]);
        send_byte(pin, rgb_color[2]);
    }
    show(pin);

    ENABLE_GLOBAL_INTERRUPTS();
}

void clear_side_color(uint8_t side_num)
{
    uint8_t i;
    port_pin_t pin = get_side_led_pin(side_num);

    DISABLE_GLOBAL_INTERRUPTS();

    for (i = 0; i < SIDE_LED_COUNT; i++)
    {
        send_byte(pin, 0);
        send_byte(pin, 0);
        send_byte(pin, 0);
    }
    show(pin);

    ENABLE_GLOBAL_INTERRUPTS();
}
