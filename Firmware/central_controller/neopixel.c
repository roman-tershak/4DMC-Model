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

/*#ifndef __builtin_avr_delay_cycles
void __builtin_avr_delay_cycles(uint32_t __n) {
    while(__n)
        __n--;
}
#endif*/

static const uint8_t COLOR_MATRIX[MAIN_COLOR_COUNT][3] = 
{
    /* Main colors */
/*    { 0,  0, 175},   // blue
    { 0, 145, 80},   // light blue
    { 0, 170,  0},   // green
    {175, 175,  0},   // yellow
    {215, 80,  0},   // orange
    {150,  0,  0},   // red
    {150,  0, 75},   // pink
    {150, 50, 200},   // purple*/
    /* Looks like the below colors are the best so far */
    { 0,  0, 140},   // blue
    { 0, 116, 64},   // light blue
    { 0, 136,  0},   // green
    {140, 140,  0},   // yellow
    {172, 60,  0},   // orange
    {120,  0,  0},   // red
    {120,  0, 60},   // pink
    {120, 40, 160},   // purple
/*    { 0,  0, 105},   // blue
    { 0, 87, 48},   // light blue
    { 0, 102,  0},   // green
    {105, 105,  0},   // yellow
    {129, 48,  0},   // orange
    {90,  0,  0},   // red
    {90,  0, 45},   // pink
    {90, 30, 120},   // purple*/
/*    { 0,  0, 70},   // blue
    { 0, 58, 32},   // light blue
    { 0, 68,  0},   // green
    {70, 70,  0},   // yellow
    {86, 32,  0},   // orange
    {60,  0,  0},   // red
    {60,  0, 30},   // pink
    {60, 20, 80},   // purple*/
    /* Main colors */
/*    { 0,  0, 35},   // blue
    { 0, 29, 16},   // light blue
    { 0, 34,  0},   // green
    {35, 35,  0},   // yellow
    {43, 16,  0},   // orange
    {30,  0,  0},   // red
    {30,  0, 15},   // pink
    {30, 10, 40},   // purple*/
};


// Actually send a bit to the string. We turn off optimizations to make sure the compile does
// not reorder things and make it so the delay happens in the wrong place.
static void send_byte(uint8_t byte_val) __attribute__ ((optimize(O0)));

static void send_byte(uint8_t byte_val)
{
    uint8_t i;

    // Neopixel wants bit in highest-to-lowest order
    for (i = 0x80; i > 0; i >>= 1)
    {
        if (byte_val & i)
        {
            set_pin(LED_COLOR);
            DELAY_CYCLES(NS_TO_CYCLES(T1H) - 2);       // 1-bit width less overhead for the actual bit setting
            // Note that this delay could be longer and everything would still work
            res_pin(LED_COLOR);
            DELAY_CYCLES(NS_TO_CYCLES(T1L) - 3);       // 1-bit gap less the overhead
        }
        else
        {
            set_pin(LED_COLOR);
            DELAY_CYCLES(NS_TO_CYCLES(T0H) - 2);       // 0-bit width less overhead 
            // **************************************************************************
            // This line is really the only tight goldilocks timing in the whole program!
            // **************************************************************************
            res_pin(LED_COLOR);
            DELAY_CYCLES(NS_TO_CYCLES(T0L) - 3);       // 0-bit gap less overhead
        }
    }
}

// Unset pin (just in case) and wait long enough without sending any bits 
// to cause the pixels to latch and display the last sent frame
static void show()
{
    res_pin(LED_COLOR);
    DELAY_CYCLES(NS_TO_CYCLES(RES));
}

void light_color_buff(uint8_t* color_buff_ptr, uint8_t color_buff_len)
{
    uint8_t *rgb_color_matrix_ptr, *color_buff_end;

    color_buff_end = color_buff_ptr + color_buff_len;
DISABLE_GLOBAL_INTERRUPTS();

    while (color_buff_ptr < color_buff_end)
    {
        rgb_color_matrix_ptr = (uint8_t*) COLOR_MATRIX[*color_buff_ptr++];

        // Note: Follow the order of GRB to sent data and the high bit sent at first. 
        send_byte(rgb_color_matrix_ptr[1]);
        send_byte(rgb_color_matrix_ptr[0]);
        send_byte(rgb_color_matrix_ptr[2]);
    }
    show();
ENABLE_GLOBAL_INTERRUPTS();

}

/*** THE BEGINNING OF COLOR ADJUSTMENT DEBUG LOGIC ***/
#ifdef DEBUG_COLOR_ADJUST

uint8_t color_matrix_adjust[9][3] =
{
    { 0,  0, 140},   // blue
    { 0, 116, 64},   // light blue
    { 0, 136,  0},   // green
    {140, 140,  0},   // yellow
    {172, 64,  0},   // orange
    {120,  0,  0},   // red
    {120,  0, 60},   // pink
    {120, 40, 160},   // purple
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


    if (cma_led_move_ind)
    {
        ln1 = cma_led_num;
        if (side_num == 0) // left
            ln2 = cma_led_num > 0 ? cma_led_num - 1 : 7;
        else // right
            ln2 = cma_led_num < 7 ? cma_led_num + 1 : 0;

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

        cma_led_ind = TRUE;  // Follow the flipped color
    }

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

