#include "central_logic.h"
#include "common.h"
#include "driver.h"
#include "rotation_logic.h"
#include "storage.h"
#include "neopixel.h"

#define SAVE_IDLE_CYCLE_SPAN   150

static void reset_sides_colors(void);
static uint8_t get_initial_color_for_side(uint8_t side_num);


/************************************************************************
 * This is the main structure holding cube stickers colors
 ***********************************************************************/
Side_State sides_states[SIDE_COUNT];

static volatile uint16_t save_cycle_counter = SAVE_IDLE_CYCLE_SPAN;

static volatile uint8_t waiting_for_saving = FALSE;


void load_sides_states(void)
{
    if (load_state())
    {
        waiting_for_saving = FALSE;
    }
    else
    {
        reset_sides_colors();
        waiting_for_saving = TRUE;
    }
    save_cycle_counter = 0;
    
    sides_colors_changed();
}

void reset_cube(void)
{
    reset_sides_colors();

    waiting_for_saving = TRUE;
    save_cycle_counter = 0;

    sides_colors_changed();
}

static void reset_sides_colors(void)
{
    uint8_t sn, c, color;
    uint8_t *colors;

    for (sn = 0; sn < SIDE_COUNT; sn++)
    {
        color = get_initial_color_for_side(sn);

        colors = sides_states[sn].colors;
        for (c = 0; c < SIDE_STICKER_COUNT; c++)
        {
            colors[c] = color;
        }
    }
}

static uint8_t get_initial_color_for_side(uint8_t side_num)
{
    switch (side_num)
    {
        case SIDE_XL: return BLUE;
        case SIDE_XR: return LIGHT_BLUE;
        case SIDE_YL: return GREEN;
        case SIDE_YR: return YELLOW;
        case SIDE_ZL: return ORANGE;
        case SIDE_ZR: return RED;
        case SIDE_CF: return PINK;
        case SIDE_CB: return PURPLE;
        default: return 0;
    }
}

void rotate_side(uint8_t sw_side_num, uint8_t direction)
{
    if (direction == MOVE_NONE)
        return;

    // rotation_logic.c
    sides_states[sw_side_num].rotation_func_ptr = get_rotation_func_ptr(sw_side_num, direction);
    do_rotation(sw_side_num);

    sides_colors_changed();

    waiting_for_saving = TRUE;
    save_cycle_counter = 0;
}

void handle_cycle(void)
{
    if (waiting_for_saving && !is_saving())
    {
        if (save_cycle_counter < SAVE_IDLE_CYCLE_SPAN)
        {
            save_cycle_counter++;
        }
        else
        {
            waiting_for_saving = FALSE;
            save_cycle_counter = 0;

            // storage.c
            save_state();
        }
    }
}

/*
 *   This is one side cubies local orientation. Each side has its own orientation of XYZ axes
 *
 *                                                                      v
 *                                                                      |
 *                                                                      |
 *                                                                     .*
 *                                                                   . * .
 *            ^Y                                                   .  * .
 *            |                                                  .   *     .
 *            |  2          3                                  .    *   .
 *            |                                              .   1 *    4 _ -*
 *            |                                            .      @  _-@-   *
 *            |6          7                              .      _*--    .  @ 3  .
 *            |  0          1                          .   _ -- *  .       .*
 *              -------------->X                     . _--   . *             *.   .
 *           /                                     ._-  .          *      *      .
 *          /  4          5                    <--*                    @ 2          .
 *         Z
 *                                                                                                  (side_zr)
 *                                                                                             y   x
 *                                                 _ 13                                         \ |
 *                                 ____    ----     **                                           \| 
 *                  6__  ------                    *                       (side_yr)        z<----
 *                 *                              *                                  x___
 *                *                              *    *                                  |\
 *               *                       _12    14                                       | \z
 *              *              7 _ ----   *   --*                                        y                     z  X   (side_xr)      x  (side_cb, inverted)
 *             5__  -----      *         * |           *                 (side_xl)              |Z              \ |                   \
 *             *              * |     __15                                        ___Y          |____Y           \|___Y           Y~___\
 *                           4_ -----     _11    *                               |\              \ (side_cf)                            |
 *                              8_  ---  | *            *                        | \Z             \X                                    |
 *              *            |  *         *                                      vX                                                     vZ
 *                             *      ___16       *  ___ 10                                             ^Y
 *                            3_ ---____    -----       *                                               |
 *               *      9__  -----                     *                                                |
 *                     *                           *  *                                        X         ---->X
 *                    *                              *                                         |         \  (side_yl)
 *                *  *                          ___ 1                                          |___Z      Z
 *                  *         _____    -----                                         (side_zl)  \
 *                 2_ -----                                                                      \Y
 *
 */
static const uint8_t LED_TO_STICKERS_MATRIX[STICKER_COUNT][2] = 
{
    {SIDE_ZL, 6}, {SIDE_CB, 4}, {SIDE_XR, 2}, {SIDE_YL, 5}, // corner 1
    {SIDE_ZL, 2}, {SIDE_XL, 5}, {SIDE_CB, 6}, {SIDE_YL, 4}, // corner 2
    {SIDE_YL, 0}, {SIDE_CF, 1}, {SIDE_ZL, 3}, {SIDE_XL, 7}, // corner 3
    {SIDE_YL, 2}, {SIDE_ZR, 4}, {SIDE_CF, 5}, {SIDE_XL, 6}, // corner 4
    {SIDE_XL, 4}, {SIDE_CB, 2}, {SIDE_YL, 6}, {SIDE_ZR, 5}, // corner 5
    {SIDE_XL, 0}, {SIDE_YR, 1}, {SIDE_CB, 3}, {SIDE_ZR, 7}, // corner 6
    {SIDE_YR, 5}, {SIDE_CF, 4}, {SIDE_ZR, 6}, {SIDE_XL, 2}, // corner 7
    {SIDE_YR, 7}, {SIDE_ZL, 1}, {SIDE_CF, 0}, {SIDE_XL, 3}, // corner 8
    {SIDE_ZL, 0}, {SIDE_CB, 7}, {SIDE_XL, 1}, {SIDE_YR, 3}, // corner 9
    {SIDE_ZL, 4}, {SIDE_XR, 6}, {SIDE_CB, 5}, {SIDE_YR, 2}, // corner 10
    {SIDE_YR, 6}, {SIDE_CF, 2}, {SIDE_ZL, 5}, {SIDE_XR, 4}, // corner 11
    {SIDE_YR, 4}, {SIDE_ZR, 2}, {SIDE_CF, 6}, {SIDE_XR, 5}, // corner 12
    {SIDE_XR, 7}, {SIDE_CB, 1}, {SIDE_YR, 0}, {SIDE_ZR, 3}, // corner 13
    {SIDE_XR, 3}, {SIDE_YL, 7}, {SIDE_CB, 0}, {SIDE_ZR, 1}, // corner 14
    {SIDE_YL, 3}, {SIDE_CF, 7}, {SIDE_ZR, 0}, {SIDE_XR, 1}, // corner 15
    {SIDE_YL, 1}, {SIDE_ZL, 7}, {SIDE_CF, 3}, {SIDE_XR, 0}  // corner 16
};

void sides_colors_changed(void)
{
    // In-memory buffer for all stickers colors
    static uint8_t color_buff[STICKER_COUNT];

    uint8_t sn, i, *buff_ptr, *led2st_ptr;

    buff_ptr = color_buff;

    for (i = 0; i < STICKER_COUNT; i++)
    {
        led2st_ptr = (uint8_t*) LED_TO_STICKERS_MATRIX[i];
        *buff_ptr++ = sides_states[ led2st_ptr[0] ].colors[ led2st_ptr[1] ];
    }

    light_color_buff(color_buff, STICKER_COUNT);

#ifdef USART_DEBUG
#ifdef DEBUG_COLOR
    for (i = 0; i < SIDE_COUNT; i++)
    {
        // Send colors to USART from the logical cube data structure
        USART_transmit_buff(sides_states[i].colors, SIDE_STICKER_COUNT);
    }
    // Send colors to USART from the physical LED data structure
    USART_transmit_buff(color_buff, STICKER_COUNT);
#endif
#endif
}

