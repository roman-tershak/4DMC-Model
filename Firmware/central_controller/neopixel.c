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

#ifndef __builtin_avr_delay_cycles
void __builtin_avr_delay_cycles(uint32_t __n) {
    while(__n)
        __n--;
}
#endif

static const uint8_t COLOR_MATRIX[16][3] = 
{
    /* Main colors */
    {30,  0,  0},   // red
    { 0, 25,  0},   // green
    { 0,  0, 25},   // blue
    {50, 50,  0},   // yellow
    { 0, 15,  5},   // light blue
    {30,  0, 25},   // pink
    {50,  7,  2},   // orange ...
    {50, 50, 50},   // white
    /* Transitioning colors */
    {10,  0,  0},   // semi-red
    { 0, 20,  0},   // semi-green
    { 0,  0,  8},   // semi-blue
    {20, 20,  0},   // semi-yellow
    { 0,  7,  4},   // semi-light blue
    {10,  0,  8},   // semi-pink
    {20,  2,  1},   // semi-orange ...
    {20, 20, 20}    // semi-white
};


/*** THE BEGINNING OF COLOR ADJUSTMENT DEBUG LOGIC ***/
#ifdef DEBUG_COLOR_ADJUST

uint8_t color_matrix_adjust[9][3] =
{
    {30,  0,  0},
    { 0, 25,  0},
    { 0,  0, 25},
    {50, 50,  0},
    { 0, 15,  5},
    {30,  0, 25},
    {50,  7,  2},
    {50, 50, 50},
    {0,   0,  0}
};

static int8_t cma_led_num = 0;

static uint8_t cma_led_ind;
static uint8_t cma_red_ind;
static uint8_t cma_green_ind;
static uint8_t cma_blue_ind;
static uint8_t cma_led_move_ind;

void debug_color_adjust(uint8_t side_num, uint8_t indicators)
{
    uint8_t c0, c1, c2, ln1, ln2, c = 0xff;

    cma_led_ind = (indicators & _BV(0)) ? TRUE : FALSE;
    cma_red_ind = (indicators & _BV(1)) ? TRUE : FALSE;
    cma_green_ind = (indicators & _BV(2)) ? TRUE : FALSE;
    cma_blue_ind = (indicators & _BV(3)) ? TRUE : FALSE;
    cma_led_move_ind = (indicators & _BV(4)) ? TRUE : FALSE;


    if (cma_led_ind)
    {
        if (side_num == 0) // decrement
        {
            cma_led_num--;
            if (cma_led_num < 0) cma_led_num = 7;
        }
        else if (side_num == 1) // increment
        {
            cma_led_num++;
            if (cma_led_num > 7) cma_led_num = 0;
        }
    }

    if (cma_led_move_ind)
    {
        ln1 = cma_led_num;
        if (side_num == 0) // left
            ln2 = cma_led_num > 0 ? cma_led_num - 1 : ln2 = 7;
        else // right
            ln2 = cma_led_num < 7 ? cma_led_num + 1 : ln2 = 0;

        // Flip colors. This is needed for visual comparison
        c0 = color_matrix_adjust[ln1][0];
        c1 = color_matrix_adjust[ln1][1];
        c2 = color_matrix_adjust[ln1][2];
        color_matrix_adjust[ln1][0] = color_matrix_adjust[ln2][0];
        color_matrix_adjust[ln1][1] = color_matrix_adjust[ln2][1];
        color_matrix_adjust[ln1][2] = color_matrix_adjust[ln2][2];
        color_matrix_adjust[ln2][0] = c0;
        color_matrix_adjust[ln2][1] = c1;
        color_matrix_adjust[ln2][2] = c2;
    }

    if (cma_red_ind) c = 0;
    if (cma_green_ind) c = 1;
    if (cma_blue_ind) c = 2;

    if (c != 0xff)
    {
        if (side_num == 0) // decrement
        {
            if (color_matrix_adjust[cma_led_num][c] > 0x1) color_matrix_adjust[cma_led_num][c] -= 2;
        }
        else if (side_num == 1) // increment
        {
            if (color_matrix_adjust[cma_led_num][c] < 0xfe) color_matrix_adjust[cma_led_num][c] += 2;
        }
    }
}

#endif
/*** THE END OF COLOR ADJUSTMENT DEBUG LOGIC ***/


// Actually send a bit to the string. We turn off optimizations to make sure the compile does
// not reorder things and make it so the delay happens in the wrong place.
static void send_byte(uint8_t pin_mask, uint8_t byte_val) __attribute__ ((optimize(0)));

static void send_byte(uint8_t pin_mask, uint8_t byte_val)
{
    uint8_t i;

    // Neopixel wants bit in highest-to-lowest order
    for (i = 0x80; i > 0; i >>= 1)
    {
        if (byte_val & i)
        {
            set_bit_mask(SC_PORT, pin_mask);
            DELAY_CYCLES(NS_TO_CYCLES(T1H) - 3);       // 1-bit width less  overhead for the actual bit setting
            // Note that this delay could be longer and everything would still work
            unset_bit_mask(SC_PORT, pin_mask);
            DELAY_CYCLES(NS_TO_CYCLES(T1L) - 4);       // TODO 1-bit gap less the overhead
        }
        else
        {
            set_bit_mask(SC_PORT, pin_mask);
            DELAY_CYCLES(NS_TO_CYCLES(T0H) - 3);       // 0-bit width less overhead 
            // **************************************************************************
            // This line is really the only tight goldilocks timing in the whole program!
            // **************************************************************************
            unset_bit_mask(SC_PORT, pin_mask);
            DELAY_CYCLES(NS_TO_CYCLES(T0L) - 4);       // TODO 0-bit gap less overhead
        }
    }
}

// Unset pin (just in case) and wait long enough without sending any bots 
// to cause the pixels to latch and display the last sent frame
static void show(uint8_t pin_mask)
{
    unset_bit_mask(SC_PORT, pin_mask);
    DELAY_CYCLES(NS_TO_CYCLES(RES));               
}

void light_side_color(uint8_t side_num, uint8_t* colors)
{
    uint8_t i;
    uint8_t *rgb_color;
    uint8_t pin_mask = _BV(get_side_led_pin(side_num));

    DISABLE_GLOBAL_INTERRUPTS();

    for (i = 0; i < SIDE_LED_COUNT; i++)
    {

#ifdef DEBUG_COLOR_ADJUST
/* DEBUG COLOR ADJUSTMENT CODE - START */
        if (i < 8)
            rgb_color = color_matrix_adjust[i];
        else
            rgb_color = color_matrix_adjust[8];
/* DEBUG COLOR ADJUSTMENT CODE - END */
#else
        rgb_color = COLOR_MATRIX[colors[i]];
#endif
        // Note: Follow the order of GRB to sent data and the high bit sent at first. 
        send_byte(pin_mask, rgb_color[1]);
        send_byte(pin_mask, rgb_color[0]);
        send_byte(pin_mask, rgb_color[2]);
    }
    show(pin_mask);

    ENABLE_GLOBAL_INTERRUPTS();
}

// TODO Do we need it?
void clear_side_color(uint8_t side_num)
{
    uint8_t i;
    uint8_t pin_mask = _BV(get_side_led_pin(side_num));

    DISABLE_GLOBAL_INTERRUPTS();

    for (i = 0; i < SIDE_LED_COUNT; i++)
    {
        send_byte(pin_mask, 0);
        send_byte(pin_mask, 0);
        send_byte(pin_mask, 0);
    }
    show(pin_mask);

    ENABLE_GLOBAL_INTERRUPTS();
}
